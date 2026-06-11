const SCREEN_WIDTH = 720;
const SCREEN_HEIGHT = 480;

const screen = document.getElementById("screen");
const screenContainer = document.getElementById("screen-container");
const loading = document.getElementById("loading");
const focusPrompt = document.getElementById("focus-prompt");
const metrics = document.getElementById("metrics");
const hostStatus = document.getElementById("host-status");
const coreSelect = document.getElementById("core-select");
const runtimeSelect = document.getElementById("runtime-select");
const speedSelect = document.getElementById("speed-select");
const autobootCheck = document.getElementById("autoboot-check");
const rebootBtn = document.getElementById("reboot-btn");
const reloadBtn = document.getElementById("reload-btn");
const fd0File = document.getElementById("fd0-file");
const mountFd0Btn = document.getElementById("mount-fd0-btn");
const unmountFd0Btn = document.getElementById("unmount-fd0-btn");
const query = new URLSearchParams(window.location.search);
const showMetrics = readBoolParam(query, "stats", true);
const selectedCore = readCoreParam(query);
const useRustCore = selectedCore === "rust";
let selectedRuntime = readRuntimeParam(query);
let selectedSpeed = readNumberParam(query, "programHz", 12000000, 100000, 12000000);
let mountedMedia = {};
let audioHost;

if (!screen) {
    throw new Error("Expected #screen canvas");
}

if (!screen.transferControlToOffscreen) {
    showError("This browser does not support OffscreenCanvas. Nova VM needs worker canvas support.");
    throw new Error("OffscreenCanvas is unavailable");
}

screen.width = SCREEN_WIDTH;
screen.height = SCREEN_HEIGHT;
screen.tabIndex = 0;

const worker = new Worker(useRustCore ? "./nova-rust-worker.js" : "./nova-worker.js", { type: "module" });
const offscreen = screen.transferControlToOffscreen();

initializeControls();
if (metrics && !showMetrics) {
    metrics.style.display = "none";
}

worker.onmessage = event => {
    const message = event.data || {};
    switch (message.type) {
        case "ready":
            if (loading) loading.style.display = "none";
            if (focusPrompt) focusPrompt.style.display = "block";
            setHostStatus(`${selectedCoreLabel()} ready`);
            screen.focus();
            break;
        case "error":
            showError(message.message || "Nova VM worker failed.");
            break;
        case "log":
            console.log("[Nova Worker]", message.message);
            break;
        case "audioConfig":
            audioHost.configure(message.cpuHz);
            break;
        case "audioEvents":
            audioHost.enqueueEvents(message.events);
            break;
        case "perf":
            updateDotNetPerf(message);
            break;
        case "metrics":
            updateMetrics(message.metrics || {});
            break;
        case "status":
            updateHostStatus(message.status || {});
            break;
    }
};

worker.onerror = event => {
    showError(event.message || "Nova VM worker failed.");
};

worker.postMessage({
    type: "init",
    canvas: offscreen,
    query: window.location.search,
    href: window.location.href
}, [offscreen]);

if (screenContainer) {
    screenContainer.addEventListener("click", () => {
        void audioHost.resume();
        screen.focus();
        hideFocusPrompt();
    });
}

document.addEventListener("keydown", event => {
    if (event.isComposing) return;
    if (isControlTarget(event.target)) return;

    void audioHost.resume();
    worker.postMessage({
        type: "key",
        key: event.key,
        shift: event.shiftKey,
        control: event.ctrlKey,
        meta: event.metaKey
    });

    if (!event.ctrlKey && !event.metaKey) {
        hideFocusPrompt();
        event.preventDefault();
    }
}, { capture: true });

document.addEventListener("paste", event => {
    if (isControlTarget(event.target)) return;
    const text = event.clipboardData ? event.clipboardData.getData("text") : "";
    if (!text) return;

    void audioHost.resume();
    worker.postMessage({ type: "text", text });
    hideFocusPrompt();
    event.preventDefault();
}, { capture: true });

function initializeControls() {
    if (coreSelect) coreSelect.value = selectedCore;
    if (runtimeSelect) runtimeSelect.value = selectedRuntime;
    if (speedSelect) speedSelect.value = String(selectedSpeed);
    if (autobootCheck) autobootCheck.checked = !readBoolParam(query, "noAuto", false);

    coreSelect?.addEventListener("change", () => {
        setQueryParam("core", coreSelect.value);
        setQueryParam("rustCore", coreSelect.value === "rust" ? "1" : "0");
        window.location.reload();
    });

    runtimeSelect?.addEventListener("change", () => {
        selectedRuntime = runtimeSelect.value;
        setQueryParam("runtime", selectedRuntime);
    });

    speedSelect?.addEventListener("change", () => {
        selectedSpeed = Number(speedSelect.value) || selectedSpeed;
        setQueryParam("promptHz", String(selectedSpeed));
        setQueryParam("programHz", String(selectedSpeed));
        setQueryParam("bootHz", String(selectedSpeed));
        sendControl({
            command: "setSpeed",
            promptHz: selectedSpeed,
            programHz: selectedSpeed,
            bootHz: selectedSpeed
        });
    });

    autobootCheck?.addEventListener("change", () => {
        setQueryParam("noAuto", autobootCheck.checked ? "0" : "1");
    });

    rebootBtn?.addEventListener("click", () => {
        if (!useRustCore) {
            window.location.reload();
            return;
        }

        selectedRuntime = runtimeSelect?.value || selectedRuntime;
        setQueryParam("runtime", selectedRuntime);
        sendControl({
            command: "reboot",
            runtime: selectedRuntime,
            skipAutoboot: !(autobootCheck?.checked ?? true)
        });
        setHostStatus(`rebooting ${runtimeLabel(selectedRuntime)}`);
        screen.focus();
    });

    reloadBtn?.addEventListener("click", () => window.location.reload());

    mountFd0Btn?.addEventListener("click", async () => {
        const file = fd0File?.files && fd0File.files.length ? fd0File.files[0] : null;
        if (!file) {
            setHostStatus("FD0 has no selected image");
            return;
        }

        const bytes = await file.arrayBuffer();
        mountedMedia.fd0 = { name: file.name, size: file.size };
        sendControl({ command: "mount", drive: "fd0", name: file.name, bytes }, [bytes]);
        renderMediaStatus();
    });

    unmountFd0Btn?.addEventListener("click", () => {
        delete mountedMedia.fd0;
        sendControl({ command: "unmount", drive: "fd0" });
        renderMediaStatus();
    });

    if (!useRustCore) {
        setHostStatus(".NET browser core selected");
    }
}

function sendControl(message, transfer = []) {
    if (!useRustCore) {
        setHostStatus("control messages require the Rust core");
        return;
    }
    worker.postMessage({ type: "control", ...message }, transfer);
}

function updateHostStatus(status) {
    if (status.runtime) {
        selectedRuntime = status.runtime;
        if (runtimeSelect) runtimeSelect.value = selectedRuntime;
    }
    if (status.media) mountedMedia = status.media;

    const runtime = runtimeLabel(status.runtime || selectedRuntime);
    const media = mediaText(status.media || mountedMedia);
    const note = status.note ? ` | ${status.note}` : "";
    setHostStatus(`${selectedCoreLabel()} | ${runtime} | ${media}${note}`);
}

function renderMediaStatus() {
    setHostStatus(`${selectedCoreLabel()} | ${runtimeLabel(selectedRuntime)} | ${mediaText(mountedMedia)}`);
}

function setHostStatus(text) {
    if (hostStatus) hostStatus.textContent = text;
}

function mediaText(media) {
    const fd0 = media && media.fd0;
    return fd0 ? `FD0 ${fd0.name} (${formatBytes(fd0.size || 0)})` : "FD0 empty";
}

function runtimeLabel(value) {
    switch (value) {
        case "logo": return "Logo";
        case "forth": return "Forth";
        case "basic": return "BASIC";
        default: return value || "BASIC";
    }
}

function selectedCoreLabel() {
    return useRustCore ? "Rust core" : ".NET core";
}

function formatBytes(value) {
    const bytes = Number(value || 0);
    if (bytes >= 1024 * 1024) return `${(bytes / 1024 / 1024).toFixed(1)} MB`;
    if (bytes >= 1024) return `${Math.round(bytes / 1024)} KB`;
    return `${bytes} B`;
}

function isControlTarget(target) {
    if (!(target instanceof Element)) return false;
    return !!target.closest("button,select,input,label");
}

function setQueryParam(name, value) {
    const params = new URLSearchParams(window.location.search);
    if (value === null || value === undefined || value === "") {
        params.delete(name);
    } else {
        params.set(name, value);
    }
    const next = `${window.location.pathname}?${params.toString()}${window.location.hash}`;
    window.history.replaceState(null, "", next);
}

function hideFocusPrompt() {
    if (focusPrompt) focusPrompt.style.display = "none";
}

function showError(message) {
    console.error("[Nova Browser]", message);
    setHostStatus(`failed: ${message}`);
    if (loading) {
        loading.textContent = `Nova VM failed: ${message}`;
        loading.style.display = "block";
    }
}

function updateDotNetPerf(message) {
    updatePerfModel(Number(message.mhz) || 0, message.cycles || 0, message.elapsedMs || 0);
    if (metrics && showMetrics) {
        metrics.textContent = `CPU ${(Number(message.mhz) || 0).toFixed(2)} MHz`;
    }
}

function updateMetrics(data) {
    const mhz = (Number(data.cpuHz || 0) / 1000000);
    updatePerfModel(mhz, data.cpuHz || 0, 1000);
    if (!metrics || !showMetrics) return;

    const timing = (Number(data.timingHz || 0) / 1000000).toFixed(1);
    const cpuAvg = Number(data.cpuMsAvg || 0).toFixed(2);
    const cpuMax = Number(data.cpuMsMax || 0).toFixed(2);
    const renderAvg = Number(data.renderMsAvg || 0).toFixed(2);
    const renderMax = Number(data.renderMsMax || 0).toFixed(2);

    metrics.textContent =
        `CPU ${mhz.toFixed(2)} MHz @${timing} | VBlank ${data.vblankHz || 0} Hz | ` +
        `Render ${data.renderHz || 0} Hz ${renderAvg}/${renderMax} ms | ` +
        `CPU slice ${cpuAvg}/${cpuMax} ms | ${data.packetKBps || 0} KB/s | ` +
        `packets F:${data.fullPackets || 0} R:${data.rowPackets || 0} ` +
        `A:${data.activePackets || 0} E:${data.emptyPackets || 0}`;
}

function updatePerfModel(mhz, cycles, elapsedMs) {
    globalThis.novaPerf = { mhz, cycles, elapsedMs };
}

function readBoolParam(params, name, fallback) {
    const raw = params.get(name);
    if (raw === null) return fallback;
    return raw === "" || raw === "1" || raw.toLowerCase() === "true" || raw.toLowerCase() === "yes";
}

function readCoreParam(params) {
    const core = (params.get("core") || "").toLowerCase();
    if (core === "dotnet" || core === "csharp" || core === "cs" || core === "wasm") return "dotnet";
    if (core === "rust") return "rust";
    return readBoolParam(params, "rustCore", true) ? "rust" : "dotnet";
}

function readRuntimeParam(params) {
    const runtime = (params.get("runtime") || "basic").toLowerCase();
    if (runtime === "novabasic" || runtime === "ehbasic") return "basic";
    if (runtime === "novalogo") return "logo";
    if (runtime === "novaforth") return "forth";
    if (["basic", "logo", "forth"].includes(runtime)) return runtime;
    return "basic";
}

function readNumberParam(params, name, fallback, min, max) {
    const raw = params.get(name);
    if (!raw) return fallback;
    const value = Number(raw);
    if (!Number.isFinite(value)) return fallback;
    return Math.min(max, Math.max(min, value));
}

class NovaAudioHost {
    constructor() {
        this.context = null;
        this.node = null;
        this.initializing = null;
        this.ready = false;
        this.enabled = false;
        this.cpuHz = 1000000;
        this.pendingBatches = [];
    }

    configure(cpuHz) {
        if (Number.isFinite(cpuHz)) {
            this.cpuHz = Math.max(100000, Math.min(12000000, cpuHz));
        }

        if (this.node) {
            this.node.port.postMessage({ type: "configure", cpuHz: this.cpuHz });
        }
    }

    enqueueEvents(events) {
        if (!events || events.length === 0) return;

        if (this.ready && this.node) {
            this.postEvents(events);
            return;
        }

        this.pendingBatches.push(events);
        while (this.pendingBatches.length > 512) {
            this.pendingBatches.shift();
        }
    }

    async resume() {
        const AudioContextCtor = window.AudioContext || window.webkitAudioContext;
        if (!AudioContextCtor) return;

        this.enableEventCollection();
        if (!this.initializing) {
            this.initializing = this.initialize(AudioContextCtor);
        }

        try {
            await this.initializing;
            if (this.context && this.context.state !== "running") {
                await this.context.resume();
            }
        } catch (error) {
            console.warn("[Nova Audio]", error);
        }
    }

    enableEventCollection() {
        if (this.enabled) return;
        this.enabled = true;
        worker.postMessage({ type: "audioEnable" });
    }

    async initialize(AudioContextCtor) {
        if (this.context) return;

        const context = new AudioContextCtor({ latencyHint: "interactive" });
        await context.audioWorklet.addModule(new URL("./nova-audio-worklet.js", import.meta.url));

        const node = new AudioWorkletNode(context, "nova-audio-processor", {
            numberOfInputs: 0,
            numberOfOutputs: 1,
            outputChannelCount: [2],
            processorOptions: { cpuHz: this.cpuHz }
        });

        node.port.onmessage = event => {
            const message = event.data || {};
            if (message.type === "error") {
                console.warn("[Nova Audio]", message.message || "audio processor error");
            }
        };
        node.connect(context.destination);

        this.context = context;
        this.node = node;
        this.ready = true;
        this.configure(this.cpuHz);
        this.flushPending();
    }

    flushPending() {
        if (!this.node) return;

        for (const events of this.pendingBatches) {
            this.postEvents(events);
        }
        this.pendingBatches.length = 0;
    }

    postEvents(events) {
        if (!this.node) return;

        try {
            if (events.buffer instanceof ArrayBuffer) {
                this.node.port.postMessage({ type: "events", events }, [events.buffer]);
            } else {
                this.node.port.postMessage({ type: "events", events });
            }
        } catch {
            this.node.port.postMessage({ type: "events", events });
        }
    }
}

audioHost = new NovaAudioHost();
