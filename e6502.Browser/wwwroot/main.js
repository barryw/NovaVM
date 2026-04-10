import { dotnet } from './_framework/dotnet.js'

const is_browser = typeof window != "undefined";
if (!is_browser) throw new Error(`Expected to be running in a browser`);

// ── Nova Audio bridge (Web Audio API ↔ C# SID/WTS) ─────────────────

let audioCtx = null;
let scriptNode = null;
let renderCallback = null;
let audioInitialized = false;
const BUFFER_SIZE = 4096;

globalThis.novaAudio = {
    init: function (sampleRate) {
        if (audioInitialized) return;
        try {
            audioCtx = new (window.AudioContext || window.webkitAudioContext)({
                sampleRate: sampleRate || 44100
            });
            this._tryConnect();
        } catch (e) {
            console.error("[Nova Audio] Failed to create AudioContext:", e);
        }
    },

    runtimeLoaded: function () {
        console.log("[Nova Audio] .NET runtime ready");
        if (audioCtx && !audioInitialized) {
            globalThis.novaAudio._tryConnect();
        }
        // Start the canvas rendering loop
        startCanvasRenderer();
    },

    _tryConnect: function () {
        if (audioInitialized || !audioCtx) return;
        try {
            const runtime = globalThis.getDotnetRuntime(0);
            if (!runtime) return;

            runtime.getAssemblyExports("e6502.Browser").then(exports => {
                renderCallback = exports.e6502.Avalonia.Hardware.OpenAlRenderer.RenderMixedAudio;

                scriptNode = audioCtx.createScriptProcessor(BUFFER_SIZE, 0, 2);
                scriptNode.onaudioprocess = function (event) {
                    if (!renderCallback) return;
                    const left = event.outputBuffer.getChannelData(0);
                    const right = event.outputBuffer.getChannelData(1);
                    const frameCount = left.length;
                    try {
                        const mixed = renderCallback(frameCount);
                        for (let i = 0; i < frameCount; i++) {
                            left[i] = mixed[i * 2] || 0;
                            right[i] = mixed[i * 2 + 1] || 0;
                        }
                    } catch (e) {
                        left.fill(0);
                        right.fill(0);
                    }
                };
                scriptNode.connect(audioCtx.destination);
                audioInitialized = true;
                console.log("[Nova Audio] Connected at " + audioCtx.sampleRate + "Hz");
            }).catch(err => {
                console.error("[Nova Audio] Failed to get exports:", err);
            });
        } catch (e) {
            // Runtime not ready yet
        }
    },

    resume: function () {
        if (audioCtx && audioCtx.state === "suspended") audioCtx.resume();
    },

    pump: function () {
        if (audioCtx && audioCtx.state === "suspended") audioCtx.resume();
        if (!audioInitialized && audioCtx) globalThis.novaAudio._tryConnect();
    }
};

// ── Direct Canvas Renderer (bypasses Avalonia rendering) ────────────

let getFramebuffer = null;
let canvasCtx = null;
let imageData = null;
let canvasRunning = false;

function startCanvasRenderer() {
    if (canvasRunning) return;

    const canvas = document.getElementById('screen');
    if (!canvas) {
        console.error("[Nova Canvas] #screen canvas not found");
        return;
    }

    canvasCtx = canvas.getContext('2d', { willReadFrequently: false });
    imageData = canvasCtx.createImageData(640, 400);

    try {
        const runtime = globalThis.getDotnetRuntime(0);
        if (!runtime) {
            console.error("[Nova Canvas] Runtime not available");
            return;
        }

        runtime.getAssemblyExports("e6502.Browser").then(exports => {
            const dc = exports.e6502.Browser.Rendering.DirectCanvas;
            getFramebuffer = dc ? dc.Tick : null;
            console.log("[Nova Canvas] Tick export:", typeof getFramebuffer);

            if (!getFramebuffer) {
                console.error("[Nova Canvas] Tick export not found!");
                return;
            }

            // Hide loading indicator
            const loading = document.getElementById('loading');
            if (loading) loading.style.display = 'none';

            // Show audio prompt
            const audioPrompt = document.getElementById('audio-prompt');
            if (audioPrompt) audioPrompt.style.display = 'block';

            canvasRunning = true;
            console.log("[Nova Canvas] Direct rendering started");
            requestAnimationFrame(renderLoop);
        }).catch(err => {
            console.error("[Nova Canvas] Failed to get exports:", err);
        });
    } catch (e) {
        console.error("[Nova Canvas] Error initializing:", e);
    }
}

function renderLoop() {
    if (!canvasRunning || !getFramebuffer) {
        requestAnimationFrame(renderLoop);
        return;
    }

    try {
        const rgba = getFramebuffer();
        if (rgba && rgba.length === 640 * 400 * 4) {
            imageData.data.set(rgba);
            canvasCtx.putImageData(imageData, 0, 0);
        }
    } catch (e) {
        console.error("[Nova Canvas] Render error:", e);
    }

    requestAnimationFrame(renderLoop);
}

// Forward keyboard events to the hidden Avalonia #out div so it receives input
function setupKeyboardForwarding() {
    const screenContainer = document.getElementById('screen-container');
    const outDiv = document.getElementById('out');

    if (screenContainer) {
        // Click on the canvas container to focus the hidden Avalonia element
        screenContainer.addEventListener('click', () => {
            // Focus the Avalonia canvas element inside #out if it exists
            const avaloniaCanvas = outDiv ? outDiv.querySelector('canvas') : null;
            if (avaloniaCanvas) {
                avaloniaCanvas.focus();
            } else if (outDiv) {
                outDiv.focus();
            }
        });
    }
}

// ── Bootstrap .NET WASM runtime ─────────────────────────────────────

const dotnetRuntime = await dotnet
    .withDiagnosticTracing(false)
    .withApplicationArgumentsFromQuery()
    .create();

const config = dotnetRuntime.getConfig();

setupKeyboardForwarding();

await dotnetRuntime.runMain(config.mainAssemblyName, [globalThis.location.href]);
