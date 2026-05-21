const SCREEN_WIDTH = 720;
const SCREEN_HEIGHT = 480;

const screen = document.getElementById("screen");
const screenContainer = document.getElementById("screen-container");
const loading = document.getElementById("loading");
const focusPrompt = document.getElementById("focus-prompt");

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

const worker = new Worker("./nova-worker.js", { type: "module" });
const offscreen = screen.transferControlToOffscreen();

worker.onmessage = event => {
    const message = event.data || {};
    switch (message.type) {
        case "ready":
            if (loading) loading.style.display = "none";
            if (focusPrompt) focusPrompt.style.display = "block";
            screen.focus();
            break;
        case "error":
            showError(message.message || "Nova VM worker failed.");
            break;
        case "log":
            console.log("[Nova Worker]", message.message);
            break;
    }
};

worker.onerror = event => {
    showError(event.message || "Nova VM worker failed.");
};

worker.postMessage({ type: "init", canvas: offscreen }, [offscreen]);

if (screenContainer) {
    screenContainer.addEventListener("click", () => {
        screen.focus();
        hideFocusPrompt();
    });
}

document.addEventListener("keydown", event => {
    if (event.isComposing) return;

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
    const text = event.clipboardData ? event.clipboardData.getData("text") : "";
    if (!text) return;

    worker.postMessage({ type: "text", text });
    hideFocusPrompt();
    event.preventDefault();
}, { capture: true });

function hideFocusPrompt() {
    if (focusPrompt) focusPrompt.style.display = "none";
}

function showError(message) {
    console.error("[Nova Browser]", message);
    if (loading) {
        loading.textContent = `Nova VM failed: ${message}`;
        loading.style.display = "block";
    }
}
