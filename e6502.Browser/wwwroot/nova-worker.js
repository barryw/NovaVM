import { dotnet } from "./_framework/dotnet.js";

const SCREEN_WIDTH = 720;
const SCREEN_HEIGHT = 480;
const TEXT_X = 40;
const TEXT_Y = 40;
const TEXT_WIDTH = 640;
const TEXT_HEIGHT = 400;
const TEXT_ROW_HEIGHT = 8;
const TEXT_ROW_BYTES = TEXT_WIDTH * TEXT_ROW_HEIGHT * 4;
const TEXT_SCREEN_BYTES = TEXT_WIDTH * TEXT_HEIGHT * 4;
const params = new URLSearchParams(self.location.search);
const RENDER_HZ = readNumberParam("renderHz", 30, 1, 60);
const PROMPT_MS = readNumberParam("promptMs", 33, 1, 100);
const PROGRAM_MS = readNumberParam("programMs", 33, 1, 100);
const BOOT_MS = readNumberParam("bootMs", 33, 1, 100);
const PROMPT_CPU_HZ = readNumberParam("promptHz", 12000000, 100000, 12000000);
const PROGRAM_CPU_HZ = readNumberParam("programHz", 12000000, 100000, 12000000);
const BOOT_CPU_HZ = readNumberParam("bootHz", 12000000, 100000, 12000000);
const TARGET_FRAME_MS = 1000 / RENDER_HZ;

let canvasContext = null;
let imageData = null;
let textScreenImageData = null;
let textRowImageData = null;
let runCpuSlice = null;
let renderFrame = null;
let renderPacket = null;
let drainExecutedCycles = null;
let queueKey = null;
let queueText = null;
let drainAudioEvents = null;
let getAudioCpuHz = null;
let setAudioEnabled = null;
let audioEnabled = false;
let running = false;
let statsCycles = 0;
let statsLastMs = 0;

self.onmessage = async event => {
    const message = event.data || {};
    switch (message.type) {
        case "init":
            await initialize(message.canvas);
            break;
        case "key":
            if (queueKey) {
                queueKey(message.key || "", !!message.shift, !!message.control, !!message.meta);
            }
            break;
        case "text":
            if (queueText && message.text) {
                queueText(message.text);
            }
            break;
        case "audioEnable":
            audioEnabled = true;
            if (setAudioEnabled) {
                setAudioEnabled(true);
            }
            break;
    }
};

async function initialize(canvas) {
    try {
        canvas.width = SCREEN_WIDTH;
        canvas.height = SCREEN_HEIGHT;

        canvasContext = canvas.getContext("2d", { willReadFrequently: false });
        if (!canvasContext) {
            throw new Error("Could not create OffscreenCanvas 2D context");
        }

        imageData = canvasContext.createImageData(SCREEN_WIDTH, SCREEN_HEIGHT);
        textScreenImageData = canvasContext.createImageData(TEXT_WIDTH, TEXT_HEIGHT);
        textRowImageData = canvasContext.createImageData(TEXT_WIDTH, TEXT_ROW_HEIGHT);

        const runtime = await dotnet
            .withDiagnosticTracing(false)
            .create();

        const config = runtime.getConfig();
        await runtime.runMain(config.mainAssemblyName, [self.location.href]);

        const exports = await runtime.getAssemblyExports("e6502.Browser");
        const directCanvas = exports.e6502.Browser.Rendering.DirectCanvas;
        runCpuSlice = directCanvas.RunCpuSlice;
        renderFrame = directCanvas.Render;
        renderPacket = directCanvas.RenderPacket;
        drainExecutedCycles = directCanvas.DrainExecutedCycles;
        directCanvas.ConfigureCpuHz(PROMPT_CPU_HZ, PROGRAM_CPU_HZ, BOOT_CPU_HZ);
        queueKey = exports.e6502.Browser.BrowserInput.QueueKey;
        queueText = exports.e6502.Browser.BrowserInput.QueueText;
        const browserAudio = exports.e6502.Browser.Audio?.BrowserAudio;
        if (browserAudio) {
            drainAudioEvents = browserAudio.DrainEvents;
            getAudioCpuHz = browserAudio.GetCpuHz;
            setAudioEnabled = browserAudio.SetEnabled;
            setAudioEnabled(false);
            postMessage({ type: "audioConfig", cpuHz: getAudioCpuHz() });
        }

        if (!runCpuSlice || !renderFrame) {
            throw new Error("DirectCanvas worker exports are unavailable");
        }

        postMessage({ type: "ready" });
        running = true;
        statsLastMs = performance.now();
        setTimeout(cpuLoop, 0);
        setTimeout(renderLoop, 0);
    } catch (error) {
        postMessage({ type: "error", message: formatError(error) });
    }
}

function cpuLoop() {
    if (!running || !runCpuSlice) return;

    try {
        const cycles = runCpuSlice(PROMPT_MS, PROGRAM_MS, BOOT_MS);
        statsCycles += Number(cycles) || 0;
        flushStats();
        flushAudioEvents();
    } catch (error) {
        running = false;
        postMessage({ type: "error", message: formatError(error) });
        return;
    }

    setTimeout(cpuLoop, 0);
}

function flushStats() {
    const now = performance.now();
    const elapsed = now - statsLastMs;
    if (elapsed < 1000) return;

    const drained = drainExecutedCycles ? Number(drainExecutedCycles()) || 0 : 0;
    const cycles = drained > 0 ? drained : statsCycles;
    const mhz = cycles / elapsed / 1000;
    postMessage({
        type: "perf",
        mhz,
        cycles,
        elapsedMs: elapsed,
        promptHz: PROMPT_CPU_HZ,
        programHz: PROGRAM_CPU_HZ,
        bootHz: BOOT_CPU_HZ
    });

    statsCycles = 0;
    statsLastMs = now;
}

function flushAudioEvents() {
    if (!audioEnabled || !drainAudioEvents) return;

    const events = drainAudioEvents();
    if (events && events.length > 0) {
        postMessage({ type: "audioEvents", events });
    }
}

function renderLoop() {
    if (!running || !renderFrame || !canvasContext || !imageData || !textScreenImageData || !textRowImageData) return;

    const started = performance.now();
    try {
        if (renderPacket) {
            applyRenderPacket(renderPacket());
        } else {
            const rgba = renderFrame();
            if (rgba && rgba.length === SCREEN_WIDTH * SCREEN_HEIGHT * 4) {
                imageData.data.set(rgba);
                canvasContext.putImageData(imageData, 0, 0);
            }
        }
    } catch (error) {
        running = false;
        postMessage({ type: "error", message: formatError(error) });
        return;
    }

    const elapsed = performance.now() - started;
    setTimeout(renderLoop, Math.max(0, TARGET_FRAME_MS - elapsed));
}

function applyRenderPacket(packet) {
    if (!packet || packet.length === 0) return;

    const packetType = packet[0];
    if (packetType === 0) {
        imageData.data.set(packet.subarray(1, 1 + SCREEN_WIDTH * SCREEN_HEIGHT * 4));
        canvasContext.putImageData(imageData, 0, 0);
        return;
    }

    if (packetType === 1) {
        const rowCount = packet[1] || 0;
        let rowOffset = 2;
        let dataOffset = 2 + rowCount;
        for (let i = 0; i < rowCount; i++) {
            const row = packet[rowOffset++];
            textRowImageData.data.set(packet.subarray(dataOffset, dataOffset + TEXT_ROW_BYTES));
            canvasContext.putImageData(textRowImageData, TEXT_X, TEXT_Y + row * TEXT_ROW_HEIGHT);
            dataOffset += TEXT_ROW_BYTES;
        }
        return;
    }

    if (packetType === 3) {
        textScreenImageData.data.set(packet.subarray(1, 1 + TEXT_SCREEN_BYTES));
        canvasContext.putImageData(textScreenImageData, TEXT_X, TEXT_Y);
    }
}

function formatError(error) {
    if (!error) return "unknown error";
    return error.stack || error.message || String(error);
}

function readNumberParam(name, fallback, min, max) {
    const raw = params.get(name);
    if (!raw) return fallback;

    const value = Number(raw);
    if (!Number.isFinite(value)) return fallback;

    return Math.min(max, Math.max(min, value));
}
