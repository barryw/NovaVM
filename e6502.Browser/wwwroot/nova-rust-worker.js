const SCREEN_WIDTH = 720;
const SCREEN_HEIGHT = 480;
const TEXT_X = 40;
const TEXT_Y = 40;
const TEXT_WIDTH = 640;
const TEXT_HEIGHT = 400;
const TEXT_ROW_HEIGHT = 8;
const TEXT_COLS = 80;
const TEXT_ROWS = 50;
const TEXT_SIZE = TEXT_COLS * TEXT_ROWS;
const GFX_WIDTH = 320;
const GFX_HEIGHT = 200;
const GFX_SIZE = GFX_WIDTH * GFX_HEIGHT;
const MAX_SPRITES = 16;
const SPRITE_REG_STRIDE = 8;
const SPRITE_SHAPE_SIZE = 128;
const SPRITE_SHAPE_RAM_SIZE = 256 * SPRITE_SHAPE_SIZE;
const TEXT_ROW_BYTES = TEXT_WIDTH * TEXT_ROW_HEIGHT * 4;
const FULL_FRAME_BYTES = SCREEN_WIDTH * SCREEN_HEIGHT * 4;
const RAM_SIZE = 65536;
const XRAM_SIZE = 0x080000;

const FIO_CMD = 0xB9A0;
const FIO_STATUS = 0xB9A1;
const FIO_ERRCODE = 0xB9A2;
const FIO_NAMELEN = 0xB9A3;
const FIO_SRCL = 0xB9A4;
const FIO_SRCH = 0xB9A5;
const FIO_ENDL = 0xB9A6;
const FIO_ENDH = 0xB9A7;
const FIO_SIZEL = 0xB9A8;
const FIO_SIZEH = 0xB9A9;
const FIO_SIZE2 = 0xB9AA;
const FIO_GSPACE = 0xB9AA;
const FIO_GADDRL = 0xB9AB;
const FIO_GADDRH = 0xB9AC;
const FIO_GLENL = 0xB9AD;
const FIO_GLENH = 0xB9AE;
const FIO_DIRTYPE = 0xB9AF;
const FIO_NAME = 0xB9B0;

const FIO_STATUS_OK = 0x02;
const FIO_STATUS_ERROR = 0x03;
const FIO_ERR_NONE = 0x00;
const FIO_ERR_NOT_FOUND = 0x01;
const FIO_ERR_IO = 0x02;
const FIO_ERR_END_OF_DIR = 0x03;
const FIO_ERR_DISK_FULL = 0x04;
const FIO_ERR_NOT_MOUNTED = 0x05;

const FIO_CMD_SAVE = 0x01;
const FIO_CMD_LOAD = 0x02;
const FIO_CMD_DIR_OPEN = 0x03;
const FIO_CMD_DIR_READ = 0x04;
const FIO_CMD_DELETE = 0x05;
const FIO_CMD_GSAVE = 0x06;
const FIO_CMD_GLOAD = 0x07;
const FIO_CMD_XLOAD = 0x18;
const FIO_CMD_XSAVE = 0x19;
const FIO_CMD_CD = 0x20;
const FIO_CMD_MKDIR = 0x21;
const FIO_CMD_RMDIR = 0x22;
const FIO_CMD_MOUNT = 0x24;
const FIO_CMD_UNMOUNT = 0x25;
const FIO_CMD_PWD = 0x26;
const FIO_CMD_LOAD_RUNTIME = 0x28;
const FIO_CMD_XPAGE = 0x29;
const FIO_CMD_NVGLOAD = 0x2B;
const FIO_CMD_FOPEN = 0x2D;
const FIO_CMD_FCREATE = 0x2E;
const FIO_CMD_FCLOSE = 0x2F;
const FIO_CMD_FREAD = 0x30;
const FIO_CMD_FWRITE = 0x31;
const FIO_CMD_FSEEK = 0x32;
const FIO_CMD_FTELL = 0x33;
const FIO_CMD_FSIZE = 0x34;
const FIO_CMD_FRESIZE = 0x35;
const FIO_CMD_FFLUSH = 0x36;
const FIO_CMD_FSTATUS = 0x37;
const FIO_CMD_FDELETE = 0x38;
const FIO_CMD_FRENAME = 0x39;

const FIO_TYPE_BAS = 0;
const FIO_TYPE_SID = 1;
const FIO_TYPE_BIN = 2;
const FIO_TYPE_MID = 3;
const FIO_TYPE_GFX = 4;
const FIO_TYPE_DIR = 5;
const FIO_TYPE_FORTH = 6;

const PAGER_TARGET_XRAM = 0;
const PAGER_TARGET_RAM = 1;
const PAGER_TARGET_VGC = 2;

const VGC_SPACE_SCREEN = 0x01;
const VGC_SPACE_COLOR = 0x02;
const VGC_SPACE_GFX = 0x03;
const VGC_SPACE_ATTR = 0x07;

const COPPER_REG_MODE = 0x00;
const COPPER_REG_BG = 0x01;
const COPPER_REG_BORDER = 0x0D;
const COPPER_REG_IRQ = 0xFE;

const C64_PALETTE = [
    [0, 0, 0], [255, 255, 255], [136, 0, 0], [170, 255, 238],
    [204, 68, 204], [0, 204, 85], [0, 0, 170], [238, 238, 119],
    [221, 136, 85], [102, 68, 0], [255, 119, 119], [51, 51, 51],
    [119, 119, 119], [170, 255, 102], [0, 136, 255], [187, 187, 187]
];

const EGA_PALETTE = [
    [0, 0, 0], [0, 0, 170], [0, 170, 0], [0, 170, 170],
    [170, 0, 0], [170, 0, 170], [170, 85, 0], [170, 170, 170],
    [85, 85, 85], [85, 85, 255], [85, 255, 85], [85, 255, 255],
    [255, 85, 85], [255, 85, 255], [255, 255, 85], [255, 255, 255]
];

let renderHz = 60;
let promptMs = 33;
let programMs = 33;
let bootMs = 33;
let promptCpuHz = 12000000;
let programCpuHz = 12000000;
let bootCpuHz = 12000000;
let targetFrameMs = 1000 / renderHz;
let statsEnabled = false;
let skipAutoboot = false;
let runtimeName = "basic";
let mountedMedia = {};

let canvasContext = null;
let fullImageData = null;
let rowImageData = null;
let wasm = null;
let exports = null;
let memory = null;
let font = null;
let runtimeAssets = null;
let extRom = null;
let libcall = null;
let modules = null;
let screen = null;
let color = null;
let attr = null;
let gfx = null;
let spriteShapes = null;
let spriteRegs = null;
let regs = null;
let ram = null;
let xram = null;
let running = false;
let basicReady = false;
let cursorVisible = true;
let cursorCounter = 0;
let lastDirtyVersion = 0;
let lastCursorX = -1;
let lastCursorY = -1;
let lastCursorEnabled = false;
let lastVideoState = "";
let lastStatsTime = 0;
let statsTimer = 0;
let stats = createStats();
let loopGeneration = 0;
let lastCpuLoopTime = 0;
let cpuCycleCredit = 0;
let defaultDrive = "fd0";
let storageDevices = { fd0: null, fd1: null, fd2: null, fd3: null };
let dirState = { entries: [], index: 0 };
let openFiles = new Array(8).fill(null);

const RUNTIMES = {
    basic: { label: "BASIC", rom: "ehbasic.bin", ext: "extension.bin" },
    logo: { label: "Logo", rom: "novalogo.bin", ext: null },
    forth: { label: "Forth", rom: "novaforth.bin", ext: null }
};

const MODULES = [
    { id: 0x01, name: "graphics", file: "graphics.bin" },
    { id: 0x03, name: "system", file: "system.bin" },
    { id: 0x02, name: "sound", file: "sound.bin" },
    { id: 0x08, name: "editor", file: "editor.bin" },
    { id: 0x04, name: "files", file: "files.bin" },
    { id: 0x05, name: "memory", file: "memory.bin" },
    { id: 0x06, name: "net", file: "net.bin" },
    { id: 0x07, name: "turtle", file: "turtle.bin" }
];

self.onmessage = async event => {
    const message = event.data || {};
    switch (message.type) {
        case "init":
            configureFromQuery(message.query || "");
            await initialize(message.canvas);
            break;
        case "key":
            queueKey(message.key || "", !!message.shift, !!message.control, !!message.meta);
            break;
        case "text":
            queueText(message.text || "");
            break;
        case "control":
            await handleControl(message);
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

        fullImageData = canvasContext.createImageData(SCREEN_WIDTH, SCREEN_HEIGHT);
        rowImageData = canvasContext.createImageData(TEXT_WIDTH, TEXT_ROW_HEIGHT);

        const [wasmBytes, fontBytes] = await Promise.all([
            fetchBytes("./rust/nova_browser_core.wasm"),
            fetchBytes("./rust/cp437.bin")
        ]);
        const runtimeEntries = await Promise.all(Object.entries(RUNTIMES).map(async ([name, runtime]) => {
            const rom = await fetchBytes(`./rust/${runtime.rom}`);
            return [name, rom];
        }));
        const [extensionBytes, libcallBytes, moduleEntries] = await Promise.all([
            fetchBytes("./rust/extension.bin"),
            fetchBytes("./rust/libcall.bin"),
            Promise.all(MODULES.map(async module => [module, await fetchBytes(`./rust/${module.file}`)]))
        ]);

        wasm = await WebAssembly.instantiate(wasmBytes, {});
        exports = wasm.instance.exports;
        memory = exports.memory;
        font = fontBytes;
        runtimeAssets = Object.fromEntries(runtimeEntries);
        extRom = extensionBytes;
        libcall = libcallBytes;
        modules = moduleEntries;

        if (!exports.nova_rom_ptr || !exports.nova_libcall_ptr || !exports.nova_module_ptr ||
            !exports.nova_ext_ptr || !exports.nova_gfx_ptr || !exports.nova_ram_ptr ||
            !exports.nova_sprite_shape_ptr || !exports.nova_sprite_regs_ptr ||
            !exports.nova_xram_ptr || !exports.nova_fio_pending_cmd || !exports.nova_fio_complete ||
            !exports.nova_copper_enabled || !exports.nova_copper_events_ptr ||
            !exports.nova_copper_event_count || !exports.nova_init || !exports.nova_run_cycles) {
            throw new Error("Rust core exports are incomplete");
        }

        bootRuntime(runtimeName, skipAutoboot);

        postMessage({ type: "ready" });
        postStatus();
        startLoops();
    } catch (error) {
        postMessage({ type: "error", message: formatError(error) });
    }
}

function loadModuleSlot(slot, bytes) {
    const ptr = exports.nova_module_ptr(slot);
    if (!ptr) {
        throw new Error(`Rust core rejected module slot ${slot}`);
    }
    new Uint8Array(memory.buffer, ptr, bytes.length).set(bytes);
}

function bootRuntime(name, noAuto) {
    const normalized = normalizeRuntime(name);
    const romBytes = runtimeAssets[normalized];
    if (!romBytes) {
        throw new Error(`Runtime '${name}' is not available`);
    }

    runtimeName = normalized;
    skipAutoboot = !!noAuto;
    new Uint8Array(memory.buffer, exports.nova_rom_ptr(), romBytes.length).set(romBytes);
    const extBytes = RUNTIMES[normalized].ext ? extRom : new Uint8Array(0);
    const extTarget = new Uint8Array(memory.buffer, exports.nova_ext_ptr(), 0x4000);
    extTarget.fill(0);
    if (extBytes.length) {
        extTarget.set(extBytes.subarray(0, Math.min(extBytes.length, extTarget.length)));
    }
    new Uint8Array(memory.buffer, exports.nova_libcall_ptr(), libcall.length).set(libcall);
    for (let i = 0; i < modules.length; i++) {
        loadModuleSlot(i, modules[i][1]);
    }
    exports.nova_init(romBytes.length, skipAutoboot ? 1 : 0);
    resetRuntimeState();
    refreshViews();
    drawFullFrame();
    postStatus();
}

function resetRuntimeState() {
    basicReady = false;
    cursorVisible = true;
    cursorCounter = 0;
    lastDirtyVersion = 0;
    lastCursorX = -1;
    lastCursorY = -1;
    lastCursorEnabled = false;
    lastVideoState = "";
    stats = createStats();
    lastStatsTime = performance.now();
    lastCpuLoopTime = 0;
    cpuCycleCredit = 0;
    dirState = { entries: [], index: 0 };
    openFiles = new Array(8).fill(null);
}

function startLoops() {
    running = true;
    loopGeneration++;
    if (statsEnabled && !statsTimer) {
        statsTimer = setInterval(reportStats, 1000);
    }
    setTimeout(() => cpuLoop(loopGeneration), 0);
    setTimeout(() => renderLoop(loopGeneration), 0);
}

function stopLoops() {
    running = false;
    loopGeneration++;
}

async function handleControl(message) {
    try {
        switch (message.command) {
            case "reboot":
                stopLoops();
                bootRuntime(message.runtime || runtimeName, !!message.skipAutoboot);
                startLoops();
                break;
            case "mount":
            {
                const drive = normalizeDrive(message.drive || "fd0");
                const bytes = message.bytes ? new Uint8Array(message.bytes) : null;
                if (!bytes || bytes.length === 0) {
                    throw new Error(`No image bytes supplied for ${drive.toUpperCase()}`);
                }
                storageDevices[drive] = new NdiDisk(bytes, message.name || "disk.ndi");
                defaultDrive = drive;
                mountedMedia[drive] = {
                    name: message.name || "disk.ndi",
                    size: bytes.byteLength || bytes.length || 0,
                    modified: false
                };
                postStatus(`${drive.toUpperCase()} mounted.`);
                break;
            }
            case "unmount":
            {
                const drive = normalizeDrive(message.drive || "fd0");
                storageDevices[drive] = null;
                delete mountedMedia[drive];
                if (defaultDrive === drive) {
                    defaultDrive = firstMountedDrive() || "fd0";
                }
                postStatus(`${drive.toUpperCase()} unmounted.`);
                break;
            }
            case "setSpeed":
                promptCpuHz = clampNumber(message.promptHz, 100000, 12000000, promptCpuHz);
                programCpuHz = clampNumber(message.programHz, 100000, 12000000, programCpuHz);
                bootCpuHz = clampNumber(message.bootHz, 100000, 12000000, bootCpuHz);
                postStatus();
                break;
            default:
                postStatus(`Unknown control command: ${message.command || ""}`);
                break;
        }
    } catch (error) {
        postMessage({ type: "error", message: formatError(error) });
    }
}

function postStatus(note = "") {
    postMessage({
        type: "status",
        status: {
            core: "rust",
            runtime: runtimeName,
            runtimeLabel: RUNTIMES[runtimeName] ? RUNTIMES[runtimeName].label : runtimeName,
            skipAutoboot,
            media: mountedMedia,
            note
        }
    });
}

async function fetchBytes(path) {
    const response = await fetch(path);
    if (!response.ok) {
        throw new Error(`${path} returned ${response.status}`);
    }
    return new Uint8Array(await response.arrayBuffer());
}

function serviceFio() {
    const command = exports.nova_fio_pending_cmd ? (exports.nova_fio_pending_cmd() & 0xFF) : 0;
    if (!command) return;

    let status = FIO_STATUS_OK;
    let error = FIO_ERR_NONE;
    try {
        handleFioCommand(command);
    } catch (exception) {
        status = FIO_STATUS_ERROR;
        error = exception instanceof FioError ? exception.code : FIO_ERR_IO;
    }
    exports.nova_fio_complete(status, error);
}

function handleFioCommand(command) {
    switch (command) {
        case FIO_CMD_SAVE:
            fioSave();
            break;
        case FIO_CMD_LOAD:
            fioLoad();
            break;
        case FIO_CMD_DIR_OPEN:
            fioDirOpen();
            break;
        case FIO_CMD_DIR_READ:
            fioDirRead();
            break;
        case FIO_CMD_DELETE:
            fioDelete(false);
            break;
        case FIO_CMD_GSAVE:
            fioGSave();
            break;
        case FIO_CMD_GLOAD:
            fioGLoad();
            break;
        case FIO_CMD_XLOAD:
            fioXLoad();
            break;
        case FIO_CMD_XSAVE:
            fioXSave();
            break;
        case FIO_CMD_CD:
            fioCd();
            break;
        case FIO_CMD_MKDIR:
            fioMkdir();
            break;
        case FIO_CMD_RMDIR:
            fioRmdir();
            break;
        case FIO_CMD_UNMOUNT:
            fioUnmount();
            break;
        case FIO_CMD_PWD:
            fioPwd();
            break;
        case FIO_CMD_LOAD_RUNTIME:
            fioLoadRuntime();
            break;
        case FIO_CMD_XPAGE:
            fioXPage();
            break;
        case FIO_CMD_NVGLOAD:
            fioNvgLoad();
            break;
        case FIO_CMD_FOPEN:
            fioFOpen(false);
            break;
        case FIO_CMD_FCREATE:
            fioFOpen(true);
            break;
        case FIO_CMD_FCLOSE:
            fioFClose();
            break;
        case FIO_CMD_FREAD:
            fioFRead();
            break;
        case FIO_CMD_FWRITE:
            fioFWrite();
            break;
        case FIO_CMD_FSEEK:
            fioFSeek();
            break;
        case FIO_CMD_FTELL:
            fioFTell();
            break;
        case FIO_CMD_FSIZE:
            fioFSize();
            break;
        case FIO_CMD_FRESIZE:
            fioFResize();
            break;
        case FIO_CMD_FFLUSH:
            fioFFlush();
            break;
        case FIO_CMD_FSTATUS:
            fioFStatus();
            break;
        case FIO_CMD_FDELETE:
            fioDelete(true);
            break;
        case FIO_CMD_FRENAME:
            fioFRename();
            break;
        case FIO_CMD_MOUNT:
            throw new FioError(FIO_ERR_IO);
        default:
            break;
    }
}

class FioError extends Error {
    constructor(code) {
        super(`FIO error ${code}`);
        this.code = code;
    }
}

function fioReg(address) {
    return ram[address] || 0;
}

function setFioReg(address, value) {
    ram[address] = value & 0xFF;
}

function fioU16(loAddress) {
    return fioReg(loAddress) | (fioReg(loAddress + 1) << 8);
}

function setFioU16(loAddress, value) {
    setFioReg(loAddress, value);
    setFioReg(loAddress + 1, value >> 8);
}

function fioTransferLength() {
    return fioReg(FIO_GLENL) | (fioReg(FIO_GLENH) << 8);
}

function fioSize24() {
    return fioReg(FIO_SIZEL) | (fioReg(FIO_SIZEH) << 8) | (fioReg(FIO_SIZE2) << 16);
}

function setFioSize24(value) {
    setFioReg(FIO_SIZEL, value);
    setFioReg(FIO_SIZEH, value >> 8);
    setFioReg(FIO_SIZE2, value >> 16);
}

function fioXramAddress() {
    return fioReg(FIO_GADDRL) | (fioReg(FIO_GADDRH) << 8) | (fioReg(FIO_GSPACE) << 16);
}

function fioFilename(raw = false) {
    const length = fioReg(FIO_NAMELEN);
    if (length < 1 || length > 63) {
        throw new FioError(FIO_ERR_IO);
    }
    let result = "";
    for (let i = 0; i < length; i++) {
        result += String.fromCharCode(fioReg(FIO_NAME + i));
    }
    result = result.trim();
    if (!result || (!raw && !/^[A-Za-z0-9_.\-/:]+$/.test(result))) {
        throw new FioError(FIO_ERR_IO);
    }
    return result;
}

function writeFioName(name) {
    const text = String(name || "").slice(0, 63);
    setFioReg(FIO_NAMELEN, text.length);
    for (let i = 0; i < text.length; i++) {
        setFioReg(FIO_NAME + i, text.charCodeAt(i));
    }
}

function fioSave() {
    const filename = fioFilename();
    const type = resolveLoadSaveType(filename) ?? fioReg(FIO_DIRTYPE);
    if (type === FIO_TYPE_FORTH) {
        const data = readCpuRange(fioU16(FIO_SRCL), checkedEndLength());
        saveDataFile(filename, ".4th", data, FIO_TYPE_FORTH);
        setFioReg(FIO_DIRTYPE, FIO_TYPE_FORTH);
        setFioSize24(data.length);
        return;
    }
    if (type !== FIO_TYPE_BAS && type !== FIO_TYPE_BIN) {
        throw new FioError(FIO_ERR_IO);
    }

    const src = fioU16(FIO_SRCL);
    const length = checkedEndLength();
    const data = new Uint8Array(length + 2);
    data[0] = src & 0xFF;
    data[1] = src >> 8;
    data.set(readCpuRange(src, length), 2);
    saveDataFile(filename, type === FIO_TYPE_BIN ? ".bin" : ".bas", data, type);
    setFioReg(FIO_DIRTYPE, type);
    setFioSize24(length);
}

function fioLoad() {
    const filename = fioFilename();
    let requestedType = resolveLoadSaveType(filename);
    if (requestedType === null) {
        const dirType = fioReg(FIO_DIRTYPE);
        requestedType = (dirType === FIO_TYPE_FORTH || dirType === FIO_TYPE_BIN) ? dirType : null;
    }

    if (requestedType === FIO_TYPE_FORTH) {
        const data = loadDataFile(filename, ".4th");
        const dest = fioU16(FIO_SRCL);
        const maxLength = fioTransferLength();
        if ((maxLength > 0 && data.length > maxLength) || !cpuRangeOk(dest, data.length)) {
            throw new FioError(FIO_ERR_IO);
        }
        ram.set(data, dest);
        setFioReg(FIO_DIRTYPE, FIO_TYPE_FORTH);
        setFioSize24(data.length);
        return;
    }

    if (requestedType === FIO_TYPE_BIN || requestedType === FIO_TYPE_BAS) {
        loadPrefixedRamFile(filename, requestedType === FIO_TYPE_BIN ? ".bin" : ".bas", requestedType, requestedType === FIO_TYPE_BIN);
        return;
    }

    try {
        loadPrefixedRamFile(filename, ".bas", FIO_TYPE_BAS, false);
    } catch (error) {
        if (!(error instanceof FioError) || error.code !== FIO_ERR_NOT_FOUND) throw error;
        loadPrefixedRamFile(filename, ".bin", FIO_TYPE_BIN, true);
    }
}

function loadPrefixedRamFile(filename, defaultExt, type, useFileLoadAddress) {
    const data = loadDataFile(filename, defaultExt);
    if (data.length < 2) throw new FioError(FIO_ERR_IO);
    const length = data.length - 2;
    const dest = useFileLoadAddress ? data[0] | (data[1] << 8) : fioU16(FIO_SRCL);
    if (!cpuRangeOk(dest, length)) throw new FioError(FIO_ERR_IO);
    if (useFileLoadAddress) setFioU16(FIO_SRCL, dest);
    ram.set(data.subarray(2), dest);
    setFioReg(FIO_DIRTYPE, type);
    setFioSize24(length);
}

function fioDirOpen() {
    const filter = fioReg(FIO_NAMELEN) > 0 ? fioFilename(true) : "";
    const entries = listDirectory(filter);
    dirState = { entries, index: 0 };
    if (entries.length === 0) throw new FioError(FIO_ERR_END_OF_DIR);
    populateDirEntry(entries[0]);
}

function fioDirRead() {
    dirState.index++;
    if (!dirState.entries || dirState.index >= dirState.entries.length) {
        throw new FioError(FIO_ERR_END_OF_DIR);
    }
    populateDirEntry(dirState.entries[dirState.index]);
}

function fioDelete(exact) {
    const filename = fioFilename();
    const resolved = resolveFile(filename, exact ? "" : ".bas");
    resolved.disk.deleteFile(resolved.name, resolved.parent);
    markMediaModified(resolved.drive);
}

function fioGSave() {
    const filename = fioFilename();
    const space = fioReg(FIO_GSPACE);
    const addr = fioU16(FIO_GADDRL);
    const length = fioTransferLength();
    const data = readVgcRange(space, addr, length);
    saveDataFile(filename, ".gfx", data, FIO_TYPE_GFX);
    setFioSize24(length);
}

function fioGLoad() {
    const filename = fioFilename();
    const data = loadDataFile(filename, ".gfx");
    const space = fioReg(FIO_GSPACE);
    const addr = fioU16(FIO_GADDRL);
    const requested = fioTransferLength();
    const length = requested > 0 ? Math.min(requested, data.length) : data.length;
    writeVgcRange(space, addr, data.subarray(0, length));
    setFioSize24(length);
}

function fioXLoad() {
    const data = loadDataFile(fioFilename(), ".xram");
    const addr = fioXramAddress();
    const requested = fioTransferLength();
    const length = requested > 0 ? Math.min(requested, data.length) : data.length;
    if (!xramRangeOk(addr, length)) throw new FioError(FIO_ERR_IO);
    xram.set(data.subarray(0, length), addr);
    setFioSize24(length);
}

function fioXSave() {
    const addr = fioXramAddress();
    const length = fioTransferLength();
    if (length <= 0 || !xramRangeOk(addr, length)) throw new FioError(FIO_ERR_IO);
    saveDataFile(fioFilename(), ".xram", xram.slice(addr, addr + length), FIO_TYPE_BIN);
    setFioSize24(length);
}

function fioXPage() {
    const data = loadDataFile(fioFilename(), ".bin");
    const requested = fioTransferLength();
    const offset = fioReg(FIO_SRCL) | (fioReg(FIO_SRCH) << 8) | (fioReg(FIO_ENDL) << 16);
    if (requested <= 0 || offset >= data.length) throw new FioError(FIO_ERR_IO);
    const length = Math.min(requested, data.length - offset);
    const slice = data.subarray(offset, offset + length);
    switch (fioReg(FIO_DIRTYPE)) {
        case PAGER_TARGET_XRAM: {
            const addr = fioXramAddress();
            if (!xramRangeOk(addr, length)) throw new FioError(FIO_ERR_IO);
            xram.set(slice, addr);
            break;
        }
        case PAGER_TARGET_RAM: {
            const addr = fioU16(FIO_GADDRL);
            if (!cpuRangeOk(addr, length)) throw new FioError(FIO_ERR_IO);
            ram.set(slice, addr);
            break;
        }
        case PAGER_TARGET_VGC:
            writeVgcRange(fioReg(FIO_GSPACE), fioU16(FIO_GADDRL), slice);
            break;
        default:
            throw new FioError(FIO_ERR_IO);
    }
    setFioSize24(length);
}

function fioCd() {
    const resolved = resolvePath(fioFilename(true));
    resolved.disk.cd(resolved.path);
    defaultDrive = resolved.drive;
}

function fioMkdir() {
    const resolved = resolveFile(fioFilename(true), "", true);
    resolved.disk.makeDirectory(resolved.name, resolved.parent);
    markMediaModified(resolved.drive);
}

function fioRmdir() {
    const resolved = resolveFile(fioFilename(true), "", true);
    resolved.disk.removeDirectory(resolved.name, resolved.parent);
    markMediaModified(resolved.drive);
}

function fioUnmount() {
    const drive = normalizeDrive(fioFilename(true));
    storageDevices[drive] = null;
    delete mountedMedia[drive];
    if (defaultDrive === drive) defaultDrive = firstMountedDrive() || "fd0";
    postStatus(`${drive.toUpperCase()} unmounted.`);
}

function fioPwd() {
    const disk = getMountedDefaultDisk();
    writeFioName(`${defaultDrive.toUpperCase()}:${disk.currentPath}`);
}

function fioLoadRuntime() {
    const filename = fioFilename();
    const lower = filename.toLowerCase();
    const runtime = Object.entries(RUNTIMES).find(([, item]) => item.rom.toLowerCase() === lower || item.rom.toLowerCase() === `${lower}.bin`);
    if (!runtime) {
        throw new FioError(FIO_ERR_NOT_FOUND);
    }
    new Uint8Array(memory.buffer, exports.nova_rom_ptr(), runtimeAssets[runtime[0]].length).set(runtimeAssets[runtime[0]]);
    ram.set(runtimeAssets[runtime[0]], 0xC000);
    setFioSize24(runtimeAssets[runtime[0]].length);
}

function fioNvgLoad() {
    const data = loadDataFile(fioFilename(), ".nvg");
    const space = fioReg(FIO_GSPACE);
    if (space !== VGC_SPACE_GFX || data.length < 12 ||
        data[0] !== 0x4E || data[1] !== 0x56 || data[2] !== 0x47 || data[3] !== 0x31) {
        throw new FioError(FIO_ERR_IO);
    }
    gfx.fill(0);
    const dest = fioU16(FIO_GADDRL);
    const width = data[4] | (data[5] << 8);
    const height = data[6] | (data[7] << 8);
    const spanCount = data[8] | (data[9] << 8) | (data[10] << 16) | (data[11] << 24);
    if (width <= 0 || height <= 0 || width > GFX_WIDTH || height > GFX_HEIGHT) throw new FioError(FIO_ERR_IO);
    let written = 0;
    let offset = 12;
    for (let span = 0; span < spanCount; span++) {
        if (offset + 3 > data.length) throw new FioError(FIO_ERR_IO);
        let imagePos = data[offset] | (data[offset + 1] << 8);
        const length = data[offset + 2];
        offset += 3;
        if (length <= 0 || imagePos + length > width * height || offset + length > data.length) throw new FioError(FIO_ERR_IO);
        for (let i = 0; i < length; i++, imagePos++) {
            const x = imagePos % width;
            const y = Math.floor(imagePos / width);
            const target = dest + y * GFX_WIDTH + x;
            if (target < 0 || target >= GFX_SIZE) throw new FioError(FIO_ERR_IO);
            gfx[target] = data[offset + i] & 0x0F;
            written++;
        }
        offset += length;
    }
    markDirty();
    setFioSize24(written);
}

function fioFOpen(create) {
    const filename = fioFilename();
    const access = decodeFileAccess(fioReg(FIO_DIRTYPE));
    let data = new Uint8Array(0);
    if (!create) {
        data = loadExactDataFile(filename);
    }
    const id = allocateFileHandle({
        filename,
        data: new Uint8Array(data),
        position: 0,
        canRead: access.canRead,
        canWrite: access.canWrite,
        dirty: create
    });
    if (id === 0) throw new FioError(FIO_ERR_IO);
    setFioU16(FIO_SRCL, id);
    setFioSize24(data.length);
}

function fioFClose() {
    const id = fioReg(FIO_SRCL);
    const handle = getFileHandle(id);
    if (handle.dirty) saveExactDataFile(handle.filename, handle.data);
    openFiles[id - 1] = null;
}

function fioFRead() {
    const handle = getFileHandle(fioReg(FIO_SRCL));
    if (!handle.canRead) throw new FioError(FIO_ERR_IO);
    const dest = fioU16(FIO_ENDL);
    const requested = fioTransferLength();
    const count = Math.min(requested, Math.max(0, handle.data.length - handle.position));
    if (!cpuRangeOk(dest, count)) throw new FioError(FIO_ERR_IO);
    ram.set(handle.data.subarray(handle.position, handle.position + count), dest);
    handle.position += count;
    setFioSize24(count);
}

function fioFWrite() {
    const handle = getFileHandle(fioReg(FIO_SRCL));
    if (!handle.canWrite) throw new FioError(FIO_ERR_IO);
    const src = fioU16(FIO_ENDL);
    const count = fioTransferLength();
    if (!cpuRangeOk(src, count)) throw new FioError(FIO_ERR_IO);
    const end = handle.position + count;
    if (end > handle.data.length) {
        const resized = new Uint8Array(end);
        resized.set(handle.data);
        handle.data = resized;
    }
    handle.data.set(ram.subarray(src, src + count), handle.position);
    handle.position = end;
    handle.dirty = true;
    setFioSize24(count);
}

function fioFSeek() {
    const handle = getFileHandle(fioReg(FIO_SRCL));
    handle.position = fioSize24();
}

function fioFTell() {
    setFioSize24(getFileHandle(fioReg(FIO_SRCL)).position);
}

function fioFSize() {
    setFioSize24(getFileHandle(fioReg(FIO_SRCL)).data.length);
}

function fioFResize() {
    const handle = getFileHandle(fioReg(FIO_SRCL));
    if (!handle.canWrite) throw new FioError(FIO_ERR_IO);
    const size = fioSize24();
    const resized = new Uint8Array(size);
    resized.set(handle.data.subarray(0, Math.min(size, handle.data.length)));
    handle.data = resized;
    if (handle.position > size) handle.position = size;
    handle.dirty = true;
}

function fioFFlush() {
    const handle = getFileHandle(fioReg(FIO_SRCL));
    if (handle.dirty) {
        saveExactDataFile(handle.filename, handle.data);
        handle.dirty = false;
    }
}

function fioFStatus() {
    const filename = fioFilename();
    const resolved = resolveFile(filename, "");
    if (!resolved.disk.fileExists(resolved.name, resolved.parent)) {
        throw new FioError(FIO_ERR_NOT_FOUND);
    }
    setFioU16(FIO_SRCL, 3);
}

function fioFRename() {
    const oldName = fioFilename();
    const newNameAddr = fioU16(FIO_ENDL);
    const newNameLength = fioTransferLength();
    if (!cpuRangeOk(newNameAddr, newNameLength) || newNameLength < 1 || newNameLength > 63) {
        throw new FioError(FIO_ERR_IO);
    }
    let newName = "";
    for (let i = 0; i < newNameLength; i++) {
        newName += String.fromCharCode(ram[newNameAddr + i]);
    }
    newName = newName.trim();
    if (!newName || !/^[A-Za-z0-9_.\-/:]+$/.test(newName)) throw new FioError(FIO_ERR_IO);
    const data = loadExactDataFile(oldName);
    saveExactDataFile(newName, data);
    fioDeleteByName(oldName);
}

function allocateFileHandle(handle) {
    for (let i = 0; i < openFiles.length; i++) {
        if (!openFiles[i]) {
            openFiles[i] = handle;
            return i + 1;
        }
    }
    return 0;
}

function getFileHandle(id) {
    if (id < 1 || id > openFiles.length || !openFiles[id - 1]) {
        throw new FioError(FIO_ERR_IO);
    }
    return openFiles[id - 1];
}

function decodeFileAccess(fam) {
    switch (fam & 0x03) {
        case 1:
            return { canRead: true, canWrite: false };
        case 2:
            return { canRead: false, canWrite: true };
        case 3:
            return { canRead: true, canWrite: true };
        default:
            throw new FioError(FIO_ERR_IO);
    }
}

function checkedEndLength() {
    const src = fioU16(FIO_SRCL);
    const end = fioU16(FIO_ENDL);
    if (end <= src) throw new FioError(FIO_ERR_IO);
    return end - src;
}

function readCpuRange(address, length) {
    if (!cpuRangeOk(address, length)) throw new FioError(FIO_ERR_IO);
    return ram.slice(address, address + length);
}

function cpuRangeOk(address, length) {
    return Number.isInteger(address) && Number.isInteger(length) && address >= 0 && length >= 0 && address + length <= RAM_SIZE;
}

function xramRangeOk(address, length) {
    return Number.isInteger(address) && Number.isInteger(length) && address >= 0 && length >= 0 && address + length <= XRAM_SIZE;
}

function readVgcRange(space, address, length) {
    const view = vgcSpaceView(space);
    if (length <= 0 || address < 0 || address + length > view.length) throw new FioError(FIO_ERR_IO);
    return view.slice(address, address + length);
}

function writeVgcRange(space, address, data) {
    const view = vgcSpaceView(space);
    if (address < 0 || address + data.length > view.length) throw new FioError(FIO_ERR_IO);
    view.set(data, address);
    markDirty();
}

function vgcSpaceView(space) {
    switch (space) {
        case VGC_SPACE_SCREEN:
            return screen;
        case VGC_SPACE_COLOR:
            return color;
        case VGC_SPACE_GFX:
            return gfx;
        case VGC_SPACE_ATTR:
            return attr;
        default:
            throw new FioError(FIO_ERR_IO);
    }
}

function markDirty() {
    if (exports.nova_mark_dirty) exports.nova_mark_dirty();
}

function loadDataFile(filename, defaultExt) {
    const resolved = resolveFile(filename, defaultExt);
    return resolved.disk.readFile(resolved.name, resolved.parent);
}

function saveDataFile(filename, defaultExt, data, fallbackType = FIO_TYPE_BIN) {
    const resolved = resolveFile(filename, defaultExt, true);
    const type = typeFromFilename(resolved.name) ?? fallbackType;
    resolved.disk.writeFile(resolved.name, type, resolved.parent, data);
    markMediaModified(resolved.drive);
}

function loadExactDataFile(filename) {
    const resolved = resolveFile(filename, "");
    return resolved.disk.readFile(resolved.name, resolved.parent);
}

function saveExactDataFile(filename, data) {
    const resolved = resolveFile(filename, "", true);
    resolved.disk.writeFile(resolved.name, typeFromFilename(resolved.name) ?? FIO_TYPE_BIN, resolved.parent, data);
    markMediaModified(resolved.drive);
}

function fioDeleteByName(filename) {
    const resolved = resolveFile(filename, "");
    resolved.disk.deleteFile(resolved.name, resolved.parent);
    markMediaModified(resolved.drive);
}

function listDirectory(filter) {
    const resolved = filter ? resolveFilter(filter) : { ...resolvePath(""), pattern: "*" };
    const entries = resolved.disk.listDirectory(resolved.parent);
    return entries
        .map(entry => storageEntry(resolved.disk, entry))
        .filter(entry => globMatch(resolved.pattern || "*", entry.matchName))
        .filter(entry => !resolved.ext || entry.isDirectory || normalizeExt(entry.ext) === normalizeExt(resolved.ext));
}

function populateDirEntry(entry) {
    const type = entry.isDirectory ? FIO_TYPE_DIR : entry.type;
    setFioReg(FIO_DIRTYPE, type);
    writeFioName(entry.displayName);
    const adjustedSize = type === FIO_TYPE_BAS || type === FIO_TYPE_BIN ? Math.max(0, entry.size - 2) : entry.size;
    setFioSize24(adjustedSize);
}

function storageEntry(disk, entry) {
    const ext = extensionOf(entry.name);
    const baseName = stripExtension(entry.name);
    return {
        isDirectory: entry.isDirectory,
        type: entry.isDirectory ? FIO_TYPE_DIR : entry.type,
        displayName: entry.isDirectory ? entry.name : baseName,
        matchName: entry.isDirectory ? entry.name : baseName,
        ext,
        size: entry.isDirectory ? 0 : entry.size,
        disk
    };
}

function resolveFilter(raw) {
    const resolved = resolvePath(raw);
    let parent = resolved.disk.currentParent;
    let pattern = resolved.path;
    if (pattern.includes("/")) {
        const split = resolved.disk.resolveParentAndName(pattern, false);
        parent = split.parent;
        pattern = split.name || "*";
    }
    if (!pattern) pattern = "*";
    const ext = extensionOf(pattern);
    return {
        disk: resolved.disk,
        drive: resolved.drive,
        parent,
        pattern: ext ? stripExtension(pattern) : pattern,
        ext
    };
}

function resolveFile(raw, defaultExt, allowCreate = false) {
    const resolved = resolvePath(raw);
    const split = resolved.disk.resolveParentAndName(resolved.path, allowCreate);
    let name = split.name;
    if (!name) throw new FioError(FIO_ERR_IO);
    if (defaultExt && !extensionOf(name)) name += defaultExt;
    return { disk: resolved.disk, drive: resolved.drive, parent: split.parent, name };
}

function resolvePath(raw) {
    let text = String(raw || "");
    let drive = defaultDrive;
    const colon = text.indexOf(":");
    if (colon > 0) {
        drive = normalizeDrive(text.slice(0, colon));
        text = text.slice(colon + 1);
    }
    const disk = storageDevices[drive];
    if (!disk) throw new FioError(FIO_ERR_NOT_MOUNTED);
    return { disk, drive, path: text };
}

function getMountedDefaultDisk() {
    const disk = storageDevices[defaultDrive] || storageDevices[firstMountedDrive() || ""];
    if (!disk) throw new FioError(FIO_ERR_NOT_MOUNTED);
    return disk;
}

function firstMountedDrive() {
    return Object.keys(storageDevices).find(key => !!storageDevices[key]) || null;
}

function normalizeDrive(value) {
    const normalized = String(value || "fd0").trim().toLowerCase();
    if (!Object.prototype.hasOwnProperty.call(storageDevices, normalized)) {
        throw new FioError(FIO_ERR_IO);
    }
    return normalized;
}

function markMediaModified(drive) {
    const media = mountedMedia[drive];
    if (media) {
        media.modified = true;
        media.size = storageDevices[drive]?.bytes.length || media.size || 0;
    }
}

function resolveLoadSaveType(filename) {
    const type = typeFromFilename(filename);
    return type === undefined ? null : type;
}

function typeFromFilename(filename) {
    switch (normalizeExt(extensionOf(filename))) {
        case ".bas":
            return FIO_TYPE_BAS;
        case ".sid":
            return FIO_TYPE_SID;
        case ".bin":
        case ".xram":
            return FIO_TYPE_BIN;
        case ".mid":
        case ".nms":
            return FIO_TYPE_MID;
        case ".gfx":
        case ".nvg":
            return FIO_TYPE_GFX;
        case ".4th":
        case ".fth":
        case ".fs":
        case ".fr":
            return FIO_TYPE_FORTH;
        case "":
            return undefined;
        default:
            return FIO_TYPE_BIN;
    }
}

function extensionOf(filename) {
    const slash = Math.max(filename.lastIndexOf("/"), filename.lastIndexOf(":"));
    const dot = filename.lastIndexOf(".");
    return dot > slash ? filename.slice(dot).toLowerCase() : "";
}

function stripExtension(filename) {
    const ext = extensionOf(filename);
    return ext ? filename.slice(0, -ext.length) : filename;
}

function normalizeExt(ext) {
    const lower = String(ext || "").toLowerCase();
    if (lower === ".fth" || lower === ".fs" || lower === ".fr") return ".4th";
    if (lower === ".nms") return ".mid";
    if (lower === ".nvg") return ".gfx";
    return lower;
}

function globMatch(pattern, text) {
    let source = "";
    for (const ch of String(pattern || "*")) {
        if (ch === "*") source += ".*";
        else if (ch === "?") source += ".";
        else source += ch.replace(/[.+^${}()|[\]\\]/g, "\\$&");
    }
    const regex = new RegExp(`^${source}$`, "i");
    return regex.test(text || "");
}

class NdiDisk {
    constructor(bytes, name) {
        this.bytes = new Uint8Array(bytes);
        this.name = name;
        this.currentPath = "/";
        this.currentParent = 0xFFFF;
        this.currentParts = [];
        this.parseHeader();
    }

    parseHeader() {
        const b = this.bytes;
        if (b.length < 256 || b[0] !== 0x4E || b[1] !== 0x44 || b[2] !== 0x49 || b[3] !== 0x1A || b[4] !== 2) {
            throw new Error(`${this.name} is not a supported NDI v2 image`);
        }
        this.sectorSize = this.readU16(5);
        if (this.sectorSize !== 256) throw new Error(`${this.name} uses an unsupported sector size`);
        this.totalSectors = this.readU32(0x08);
        this.dirStart = this.readU32(0x2C);
        this.dirSectorCount = this.readU32(0x30);
        this.dataStart = this.readU32(0x34);
        this.entryCount = this.dirSectorCount * 4;
        this.bamOffset = this.sectorSize;
        this.dirOffset = this.dirStart * this.sectorSize;
        this.dataOffset = this.dataStart * this.sectorSize;
        this.dataSectorCount = this.totalSectors - this.dataStart;
        if (this.bytes.length < this.totalSectors * this.sectorSize) {
            throw new Error(`${this.name} is shorter than its NDI header declares`);
        }
    }

    readFile(name, parent) {
        const entry = this.findRequiredEntry(name, parent);
        if (entry.isDirectory) throw new FioError(FIO_ERR_IO);
        const offset = this.dataOffset + entry.start * this.sectorSize;
        return this.bytes.slice(offset, offset + entry.size);
    }

    writeFile(name, type, parent, data) {
        const old = this.findEntry(name, parent);
        if (old) {
            if (old.isDirectory) throw new FioError(FIO_ERR_IO);
            this.freeSectors(old.start, old.sectorCount);
            this.clearEntry(old.index);
        }
        const sectorCount = Math.max(1, Math.ceil(data.length / this.sectorSize));
        const start = this.allocateContiguous(sectorCount);
        if (start < 0) throw new FioError(FIO_ERR_DISK_FULL);
        const offset = this.dataOffset + start * this.sectorSize;
        this.bytes.fill(0, offset, offset + sectorCount * this.sectorSize);
        this.bytes.set(data, offset);
        this.addEntry(name.slice(0, 32), type, parent, start, data.length, sectorCount);
        this.flushFreeCount();
    }

    deleteFile(name, parent) {
        const entry = this.findRequiredEntry(name, parent);
        if (entry.isDirectory) throw new FioError(FIO_ERR_IO);
        this.freeSectors(entry.start, entry.sectorCount);
        this.clearEntry(entry.index);
        this.flushFreeCount();
    }

    fileExists(name, parent) {
        const entry = this.findEntry(name, parent);
        return !!entry && !entry.isDirectory;
    }

    listDirectory(parent) {
        const result = [];
        for (let i = 0; i < this.entryCount; i++) {
            const entry = this.readEntry(i);
            if (entry.active && entry.parent === parent) result.push(entry);
        }
        return result;
    }

    makeDirectory(name, parent) {
        if (this.findEntry(name, parent)) throw new FioError(FIO_ERR_IO);
        this.addEntry(name.slice(0, 32), FIO_TYPE_DIR, parent, 0, 0, 0, true);
    }

    removeDirectory(name, parent) {
        const entry = this.findRequiredEntry(name, parent);
        if (!entry.isDirectory || this.listDirectory(entry.index).length > 0) throw new FioError(FIO_ERR_IO);
        this.clearEntry(entry.index);
    }

    cd(path) {
        const parts = this.resolveParts(path);
        this.currentParent = this.parentForParts(parts);
        this.currentParts = parts;
        this.currentPath = parts.length ? `/${parts.join("/")}` : "/";
    }

    resolveParentAndName(path) {
        const parts = this.resolveParts(path);
        const name = parts.pop() || "";
        return { parent: this.parentForParts(parts), name };
    }

    resolveParts(path) {
        let parts = String(path || "").startsWith("/") ? [] : [...this.currentParts];
        for (const rawPart of String(path || "").split("/")) {
            const part = rawPart.trim();
            if (!part || part === ".") continue;
            if (part === "..") {
                parts.pop();
            } else {
                parts.push(part);
            }
        }
        return parts;
    }

    parentForParts(parts) {
        let parent = 0xFFFF;
        for (const part of parts) {
            const entry = this.findEntry(part, parent);
            if (!entry || !entry.isDirectory) throw new FioError(FIO_ERR_IO);
            parent = entry.index;
        }
        return parent;
    }

    findRequiredEntry(name, parent) {
        const entry = this.findEntry(name, parent);
        if (!entry) throw new FioError(FIO_ERR_NOT_FOUND);
        return entry;
    }

    findEntry(name, parent) {
        const wanted = name.toLowerCase();
        for (let i = 0; i < this.entryCount; i++) {
            const entry = this.readEntry(i);
            if (entry.active && entry.parent === parent && entry.name.toLowerCase() === wanted) {
                return entry;
            }
        }
        return null;
    }

    addEntry(name, type, parent, start, size, sectorCount, directory = false) {
        const index = this.findFreeEntry();
        if (index < 0) throw new FioError(FIO_ERR_DISK_FULL);
        const offset = this.entryOffset(index);
        this.bytes[offset] = directory ? 0x03 : 0x01;
        this.bytes[offset + 1] = type & 0xFF;
        this.writeU16(offset + 0x02, parent);
        this.writeU32(offset + 0x04, start);
        this.writeU32(offset + 0x08, size);
        this.bytes.fill(0, offset + 0x0C, offset + 0x2C);
        for (let i = 0; i < Math.min(32, name.length); i++) this.bytes[offset + 0x0C + i] = name.charCodeAt(i) & 0x7F;
        this.writeU32(offset + 0x2C, sectorCount);
    }

    findFreeEntry() {
        for (let i = 0; i < this.entryCount; i++) {
            if (this.bytes[this.entryOffset(i)] === 0) return i;
        }
        return -1;
    }

    clearEntry(index) {
        this.bytes[this.entryOffset(index)] = 0;
    }

    readEntry(index) {
        const offset = this.entryOffset(index);
        const flags = this.bytes[offset];
        const nameBytes = this.bytes.subarray(offset + 0x0C, offset + 0x2C);
        let nameLength = nameBytes.indexOf(0);
        if (nameLength < 0) nameLength = nameBytes.length;
        let name = "";
        for (let i = 0; i < nameLength; i++) name += String.fromCharCode(nameBytes[i]);
        return {
            index,
            active: (flags & 0x01) !== 0,
            isDirectory: (flags & 0x02) !== 0,
            type: this.bytes[offset + 1],
            parent: this.readU16(offset + 0x02),
            start: this.readU32(offset + 0x04),
            size: this.readU32(offset + 0x08),
            name,
            sectorCount: this.readU32(offset + 0x2C)
        };
    }

    allocateContiguous(count) {
        let runStart = -1;
        let runLength = 0;
        for (let sector = 0; sector < this.dataSectorCount; sector++) {
            if (!this.isAllocated(sector)) {
                if (runStart < 0) runStart = sector;
                runLength++;
                if (runLength === count) {
                    for (let i = runStart; i < runStart + count; i++) this.setAllocated(i, true);
                    return runStart;
                }
            } else {
                runStart = -1;
                runLength = 0;
            }
        }
        return -1;
    }

    freeSectors(start, count) {
        for (let i = start; i < start + count; i++) this.setAllocated(i, false);
    }

    isAllocated(sector) {
        return (this.bytes[this.bamOffset + (sector >> 3)] & (1 << (sector & 7))) !== 0;
    }

    setAllocated(sector, allocated) {
        const offset = this.bamOffset + (sector >> 3);
        const bit = 1 << (sector & 7);
        if (allocated) this.bytes[offset] |= bit;
        else this.bytes[offset] &= ~bit;
    }

    flushFreeCount() {
        let free = 0;
        for (let i = 0; i < this.dataSectorCount; i++) if (!this.isAllocated(i)) free++;
        this.writeU32(0x38, free);
    }

    entryOffset(index) {
        return this.dirOffset + index * 64;
    }

    readU16(offset) {
        return this.bytes[offset] | (this.bytes[offset + 1] << 8);
    }

    readU32(offset) {
        return (this.bytes[offset] | (this.bytes[offset + 1] << 8) | (this.bytes[offset + 2] << 16) | (this.bytes[offset + 3] << 24)) >>> 0;
    }

    writeU16(offset, value) {
        this.bytes[offset] = value & 0xFF;
        this.bytes[offset + 1] = (value >> 8) & 0xFF;
    }

    writeU32(offset, value) {
        this.bytes[offset] = value & 0xFF;
        this.bytes[offset + 1] = (value >> 8) & 0xFF;
        this.bytes[offset + 2] = (value >> 16) & 0xFF;
        this.bytes[offset + 3] = (value >> 24) & 0xFF;
    }
}

function refreshViews() {
    ram = new Uint8Array(memory.buffer, exports.nova_ram_ptr(), RAM_SIZE);
    xram = new Uint8Array(memory.buffer, exports.nova_xram_ptr(), XRAM_SIZE);
    screen = new Uint8Array(memory.buffer, exports.nova_screen_ptr(), TEXT_SIZE);
    color = new Uint8Array(memory.buffer, exports.nova_color_ptr(), TEXT_SIZE);
    attr = new Uint8Array(memory.buffer, exports.nova_attr_ptr(), TEXT_SIZE);
    gfx = new Uint8Array(memory.buffer, exports.nova_gfx_ptr(), GFX_SIZE);
    spriteShapes = new Uint8Array(memory.buffer, exports.nova_sprite_shape_ptr(), SPRITE_SHAPE_RAM_SIZE);
    spriteRegs = new Uint8Array(memory.buffer, exports.nova_sprite_regs_ptr(), MAX_SPRITES * SPRITE_REG_STRIDE);
    regs = new Uint8Array(memory.buffer, exports.nova_regs_ptr(), 16);
}

function cpuLoop(generation) {
    if (!running || generation !== loopGeneration) return;

    const started = performance.now();
    const promptInputMode = basicReady && !!(regs[0x0A] & 1);
    const budgetMs = !basicReady ? bootMs : promptInputMode ? promptMs : programMs;
    const targetHz = !basicReady ? bootCpuHz : promptInputMode ? promptCpuHz : programCpuHz;
    if (lastCpuLoopTime <= 0) {
        lastCpuLoopTime = started;
    }
    const elapsedSinceLastLoop = Math.max(0, started - lastCpuLoopTime);
    lastCpuLoopTime = started;
    const maxCredit = targetHz * 0.25;
    cpuCycleCredit = Math.min(maxCredit, cpuCycleCredit + targetHz * elapsedSinceLastLoop / 1000);
    const maxSliceCycles = Math.max(1, Math.floor(targetHz * Math.max(1, budgetMs) / 1000));
    const targetCycles = Math.max(1, Math.min(maxSliceCycles, Math.floor(cpuCycleCredit)));
    let cycles = 0;

    try {
        cycles = exports.nova_run_cycles(targetCycles) || 0;
        const trap = exports.nova_trap_code ? exports.nova_trap_code() : 0;
        if (trap) {
            const pc = (trap >>> 8) & 0xFFFF;
            const opcode = trap & 0xFF;
            throw new Error(`Rust CPU trapped at $${pc.toString(16).padStart(4, "0")} opcode $${opcode.toString(16).padStart(2, "0")}`);
        }
        refreshViews();
        serviceFio();
        if (!basicReady) {
            basicReady = detectReadySignal();
        }
    } catch (error) {
        running = false;
        stopStatsTimer();
        postMessage({ type: "error", message: formatError(error) });
        return;
    }

    const elapsed = performance.now() - started;
    stats.cpuLoops++;
    stats.cpuCycles += cycles;
    stats.cpuMs += elapsed;
    if (elapsed > stats.maxCpuMs) stats.maxCpuMs = elapsed;
    stats.timingHz = targetHz;
    cpuCycleCredit = Math.max(0, cpuCycleCredit - cycles);

    setTimeout(() => cpuLoop(generation), cpuCycleCredit >= 1 ? 0 : 1);
}

function renderLoop(generation) {
    if (!running || generation !== loopGeneration || !canvasContext || !rowImageData) return;

    const started = performance.now();
    try {
        if (exports.nova_vblank) {
            exports.nova_vblank();
            stats.vblankFrames++;
        }
        advanceCursorBlink();
        renderChanges();
    } catch (error) {
        running = false;
        stopStatsTimer();
        postMessage({ type: "error", message: formatError(error) });
        return;
    }

    const elapsed = performance.now() - started;
    stats.renderFrames++;
    stats.renderMs += elapsed;
    if (elapsed > stats.maxRenderMs) stats.maxRenderMs = elapsed;

    setTimeout(() => renderLoop(generation), Math.max(0, targetFrameMs - elapsed));
}

function drawFullFrame() {
    const data = fullImageData.data;
    const palette = activePalette();
    const mode = regs[0x00] || 0;
    const bg = (regs[0x01] || 0) & 0x0F;
    const border = (regs[0x0D] || 0) & 0x0F;
    const copperEvents = readCopperEvents();

    fillRect(data, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, palette[border]);

    if (copperEvents.length > 0) {
        drawCopperFrame(data, palette, copperEvents, mode, bg);
        canvasContext.putImageData(fullImageData, 0, 0);
        stats.packetType0++;
        stats.packetBytes += FULL_FRAME_BYTES;
        return;
    }

    fillRect(data, TEXT_X, TEXT_Y, TEXT_WIDTH, TEXT_HEIGHT, palette[bg]);
    drawSprites(data, palette, 0);

    if (mode === 2) {
        drawGfxLayer(data, palette);
        drawSprites(data, palette, 1);
        drawTextLayer(data, palette, true);
    } else if (mode === 3 || mode === 4) {
        drawGfxLayer(data, palette);
        drawSprites(data, palette, 1);
    } else {
        drawTextLayer(data, palette, false);
        if (mode >= 1) {
            drawSprites(data, palette, 1);
            drawGfxLayer(data, palette);
        } else {
            drawSprites(data, palette, 1);
        }
    }
    drawSprites(data, palette, 2);

    canvasContext.putImageData(fullImageData, 0, 0);
    stats.packetType0++;
    stats.packetBytes += FULL_FRAME_BYTES;
}

function fillRect(data, x, y, width, height, rgb) {
    for (let py = 0; py < height; py++) {
        let offset = ((y + py) * SCREEN_WIDTH + x) * 4;
        for (let px = 0; px < width; px++) {
            data[offset++] = rgb[0];
            data[offset++] = rgb[1];
            data[offset++] = rgb[2];
            data[offset++] = 255;
        }
    }
}

function readCopperEvents() {
    if (!isCopperActive()) {
        return [];
    }

    const count = Math.min(exports.nova_copper_event_count() >>> 0, 256);
    const ptr = exports.nova_copper_events_ptr() >>> 0;
    const view = new DataView(memory.buffer, ptr, count * 4);
    const events = [];
    for (let i = 0; i < count; i++) {
        const offset = i * 4;
        events.push({
            position: view.getUint16(offset, true),
            register: view.getUint8(offset + 2),
            value: view.getUint8(offset + 3)
        });
    }
    return events;
}

function isCopperActive() {
    return !!exports.nova_copper_enabled &&
        !!exports.nova_copper_event_count &&
        !!exports.nova_copper_events_ptr &&
        (exports.nova_copper_enabled() & 1) !== 0 &&
        (exports.nova_copper_event_count() >>> 0) > 0;
}

function drawCopperFrame(data, palette, copperEvents, initialMode, initialBg) {
    const state = { mode: initialMode, bg: initialBg };
    let eventIndex = 0;

    for (let y = 0; y < GFX_HEIGHT; y++) {
        const rowStart = y * GFX_WIDTH;
        while (eventIndex < copperEvents.length && copperEvents[eventIndex].position <= rowStart) {
            applyCopperEvent(state, copperEvents[eventIndex]);
            eventIndex++;
        }

        fillGfxScanline(data, palette[state.bg & 0x0F], y);
        drawSpritesScanline(data, palette, 0, y);

        if (state.mode === 2) {
            drawGfxScanline(data, palette, y);
            drawSpritesScanline(data, palette, 1, y);
            drawTextCanvasScanline(data, palette, y * 2, state.mode, state.bg, true);
            drawTextCanvasScanline(data, palette, y * 2 + 1, state.mode, state.bg, true);
        } else if (state.mode === 3 || state.mode === 4) {
            drawGfxScanline(data, palette, y);
            drawSpritesScanline(data, palette, 1, y);
        } else {
            drawTextCanvasScanline(data, palette, y * 2, state.mode, state.bg, false);
            drawTextCanvasScanline(data, palette, y * 2 + 1, state.mode, state.bg, false);
            drawSpritesScanline(data, palette, 1, y);
            if (state.mode >= 1) {
                drawGfxScanline(data, palette, y);
            }
        }

        drawSpritesScanline(data, palette, 2, y);
    }
}

function applyCopperEvent(state, event) {
    switch (event.register) {
        case COPPER_REG_MODE:
            state.mode = event.value;
            break;
        case COPPER_REG_BG:
            state.bg = event.value & 0x0F;
            break;
        case COPPER_REG_BORDER:
            break;
        case COPPER_REG_IRQ:
            if (exports.nova_raise_copper_irq) {
                exports.nova_raise_copper_irq(event.value);
            }
            break;
        default:
            break;
    }
}

function fillGfxScanline(data, rgb, y) {
    const destY = TEXT_Y + y * 2;
    for (let dy = 0; dy < 2; dy++) {
        let offset = ((destY + dy) * SCREEN_WIDTH + TEXT_X) * 4;
        for (let px = 0; px < TEXT_WIDTH; px++) {
            writePixel(data, offset, rgb);
            offset += 4;
        }
    }
}

function renderChanges() {
    const dirtyVersion = exports.nova_dirty_version();
    const cursorX = regs[0x03] || 0;
    const cursorY = regs[0x04] || 0;
    const cursorEnabled = !!(regs[0x0A] & 1);
    const mode = regs[0x00] || 0;
    const copperActive = isCopperActive();
    const videoState = currentVideoState();

    if (dirtyVersion === lastDirtyVersion &&
        cursorX === lastCursorX &&
        cursorY === lastCursorY &&
        cursorEnabled === lastCursorEnabled &&
        videoState === lastVideoState) {
        stats.packetType2++;
        return;
    }

    if (mode !== 0 || copperActive || videoState !== lastVideoState) {
        drawFullFrame();
    } else {
        for (let row = 0; row < TEXT_ROWS; row++) {
            drawTextRow(row);
        }
    }

    lastDirtyVersion = dirtyVersion;
    lastCursorX = cursorX;
    lastCursorY = cursorY;
    lastCursorEnabled = cursorEnabled;
    lastVideoState = videoState;
}

function drawTextRow(row) {
    const data = rowImageData.data;
    drawTextRowInto(data, row, TEXT_WIDTH, 0, 0, activePalette(), false);

    canvasContext.putImageData(rowImageData, TEXT_X, TEXT_Y + row * TEXT_ROW_HEIGHT);
    stats.packetType1++;
    stats.dirtyRows++;
    stats.packetBytes += TEXT_ROW_BYTES;
}

function drawGfxLayer(data, palette) {
    const transparent = exports.nova_gfx_transparent_color ? (exports.nova_gfx_transparent_color() & 0x0F) : 0;

    for (let y = 0; y < GFX_HEIGHT; y++) {
        const sourceRow = y * GFX_WIDTH;
        const destY = TEXT_Y + y * 2;
        const destRow0 = (destY * SCREEN_WIDTH + TEXT_X) * 4;
        const destRow1 = ((destY + 1) * SCREEN_WIDTH + TEXT_X) * 4;

        for (let x = 0; x < GFX_WIDTH; x++) {
            const colorIndex = gfx[sourceRow + x] & 0x0F;
            if (colorIndex === transparent) {
                continue;
            }

            const rgb = palette[colorIndex];
            const dx = x * 2 * 4;
            writePixel(data, destRow0 + dx, rgb);
            writePixel(data, destRow0 + dx + 4, rgb);
            writePixel(data, destRow1 + dx, rgb);
            writePixel(data, destRow1 + dx + 4, rgb);
        }
    }
}

function drawGfxScanline(data, palette, y) {
    const transparent = exports.nova_gfx_transparent_color ? (exports.nova_gfx_transparent_color() & 0x0F) : 0;
    const sourceRow = y * GFX_WIDTH;
    const destY = TEXT_Y + y * 2;
    const destRow0 = (destY * SCREEN_WIDTH + TEXT_X) * 4;
    const destRow1 = ((destY + 1) * SCREEN_WIDTH + TEXT_X) * 4;

    for (let x = 0; x < GFX_WIDTH; x++) {
        const colorIndex = gfx[sourceRow + x] & 0x0F;
        if (colorIndex === transparent) {
            continue;
        }

        const rgb = palette[colorIndex];
        const dx = x * 2 * 4;
        writePixel(data, destRow0 + dx, rgb);
        writePixel(data, destRow0 + dx + 4, rgb);
        writePixel(data, destRow1 + dx, rgb);
        writePixel(data, destRow1 + dx + 4, rgb);
    }
}

function drawSprites(data, palette, priority) {
    if (!spriteRegs || !spriteShapes) return;
    for (let sprite = 0; sprite < MAX_SPRITES; sprite++) {
        const base = sprite * SPRITE_REG_STRIDE;
        const flags = spriteRegs[base + 5] || 0;
        if ((flags & 0x80) === 0) continue;
        if (Math.min(spriteRegs[base + 6] || 0, 2) !== priority) continue;

        const sx = (spriteRegs[base] || 0) | ((spriteRegs[base + 1] || 0) << 8);
        const sy = spriteRegs[base + 2] || 0;
        const shapeBase = (spriteRegs[base + 4] || 0) * SPRITE_SHAPE_SIZE;
        const transparent = (spriteRegs[base + 7] || 0) & 0x0F;
        const xFlip = (flags & 0x01) !== 0;
        const yFlip = (flags & 0x02) !== 0;

        for (let y = 0; y < 16; y++) {
            const srcY = yFlip ? 15 - y : y;
            const py = sy + y;
            if (py < 0 || py >= GFX_HEIGHT) continue;
            const destY = TEXT_Y + py * 2;
            for (let x = 0; x < 16; x++) {
                const srcX = xFlip ? 15 - x : x;
                const px = sx + x;
                if (px < 0 || px >= GFX_WIDTH) continue;
                const packed = spriteShapes[shapeBase + srcY * 8 + (srcX >> 1)] || 0;
                const colorIndex = ((srcX & 1) === 0 ? packed >> 4 : packed) & 0x0F;
                if (colorIndex === transparent) continue;
                const rgb = palette[colorIndex];
                const destX = TEXT_X + px * 2;
                const row0 = (destY * SCREEN_WIDTH + destX) * 4;
                const row1 = ((destY + 1) * SCREEN_WIDTH + destX) * 4;
                writePixel(data, row0, rgb);
                writePixel(data, row0 + 4, rgb);
                writePixel(data, row1, rgb);
                writePixel(data, row1 + 4, rgb);
            }
        }
    }
}

function drawSpritesScanline(data, palette, priority, gfxY) {
    if (!spriteRegs || !spriteShapes) return;
    for (let sprite = 0; sprite < MAX_SPRITES; sprite++) {
        const base = sprite * SPRITE_REG_STRIDE;
        const flags = spriteRegs[base + 5] || 0;
        if ((flags & 0x80) === 0) continue;
        if (Math.min(spriteRegs[base + 6] || 0, 2) !== priority) continue;

        const sx = (spriteRegs[base] || 0) | ((spriteRegs[base + 1] || 0) << 8);
        const sy = (spriteRegs[base + 2] || 0) | ((spriteRegs[base + 3] || 0) << 8);
        const localY = gfxY - sy;
        if (localY < 0 || localY >= 16) continue;

        const shapeBase = (spriteRegs[base + 4] || 0) * SPRITE_SHAPE_SIZE;
        const transparent = (spriteRegs[base + 7] || 0) & 0x0F;
        const xFlip = (flags & 0x01) !== 0;
        const yFlip = (flags & 0x02) !== 0;
        const srcY = yFlip ? 15 - localY : localY;
        const destY = TEXT_Y + gfxY * 2;

        for (let x = 0; x < 16; x++) {
            const srcX = xFlip ? 15 - x : x;
            const px = sx + x;
            if (px < 0 || px >= GFX_WIDTH) continue;
            const packed = spriteShapes[shapeBase + srcY * 8 + (srcX >> 1)] || 0;
            const colorIndex = ((srcX & 1) === 0 ? packed >> 4 : packed) & 0x0F;
            if (colorIndex === transparent) continue;
            const rgb = palette[colorIndex];
            const destX = TEXT_X + px * 2;
            const row0 = (destY * SCREEN_WIDTH + destX) * 4;
            const row1 = ((destY + 1) * SCREEN_WIDTH + destX) * 4;
            writePixel(data, row0, rgb);
            writePixel(data, row0 + 4, rgb);
            writePixel(data, row1, rgb);
            writePixel(data, row1 + 4, rgb);
        }
    }
}

function drawTextLayer(data, palette, transparentMatchingBg) {
    for (let row = 0; row < TEXT_ROWS; row++) {
        drawTextRowInto(data, row, SCREEN_WIDTH, TEXT_X, TEXT_Y + row * TEXT_ROW_HEIGHT, palette, transparentMatchingBg);
    }
}

function drawTextCanvasScanline(data, palette, canvasPy, mode, bg, transparentMatchingBg) {
    const cursorX = regs[0x03] || 0;
    const cursorY = regs[0x04] || 0;
    const cursorEnabled = !!(regs[0x0A] & 1) && cursorVisible && mode !== 3 && mode !== 4;
    const displayRow = Math.floor(canvasPy / TEXT_ROW_HEIGHT);
    const glyphY = canvasPy % TEXT_ROW_HEIGHT;
    const destY = TEXT_Y + canvasPy;

    for (let col = 0; col < TEXT_COLS; col++) {
        const cell = displayRow * TEXT_COLS + col;
        const ch = screen[cell] || 0x20;
        const colorAttr = color[cell] || 0x0F;
        const textAttr = attr[cell] || 0;
        let fg = colorAttr & 0x0F;
        let cellBg = (colorAttr >> 4) & 0x0F;
        const isCursor = cursorEnabled && col === cursorX && displayRow === cursorY;
        if (isCursor) {
            const tmp = fg;
            fg = cellBg;
            cellBg = tmp;
        }

        const bits = (textAttr & 0x01) !== 0 && !cursorVisible ? 0 : (font[ch * 8 + glyphY] || 0);
        const fgRgb = palette[fg];
        const bgRgb = palette[cellBg];
        let offset = (destY * SCREEN_WIDTH + TEXT_X + col * 8) * 4;
        for (let glyphX = 0; glyphX < 8; glyphX++) {
            const set = (bits & (0x80 >> glyphX)) !== 0;
            if (!set && transparentMatchingBg && !isCursor && cellBg === (bg & 0x0F)) {
                offset += 4;
                continue;
            }
            writePixel(data, offset, set ? fgRgb : bgRgb);
            offset += 4;
        }
    }
}

function drawTextRowInto(data, row, stride, originX, originY, palette, transparentMatchingBg) {
    const cursorX = regs[0x03] || 0;
    const cursorY = regs[0x04] || 0;
    const mode = regs[0x00] || 0;
    const globalBg = (regs[0x01] || 0) & 0x0F;
    const cursorEnabled = !!(regs[0x0A] & 1) && cursorVisible && mode !== 3 && mode !== 4;

    for (let col = 0; col < TEXT_COLS; col++) {
        const cell = row * TEXT_COLS + col;
        const ch = screen[cell] || 0x20;
        const colorAttr = color[cell] || 0x0F;
        const textAttr = attr[cell] || 0;
        let fg = colorAttr & 0x0F;
        let bg = (colorAttr >> 4) & 0x0F;
        const isCursor = cursorEnabled && col === cursorX && row === cursorY;
        if (isCursor) {
            const tmp = fg;
            fg = bg;
            bg = tmp;
        }

        const fgRgb = palette[fg];
        const bgRgb = palette[bg];
        const flashHidden = (textAttr & 0x01) !== 0 && !cursorVisible;
        const transparentCellBg = transparentMatchingBg && !isCursor && bg === globalBg;
        for (let glyphY = 0; glyphY < 8; glyphY++) {
            const bits = flashHidden ? 0 : (font[ch * 8 + glyphY] || 0);
            let offset = ((originY + glyphY) * stride + originX + col * 8) * 4;
            for (let glyphX = 0; glyphX < 8; glyphX++) {
                const set = (bits & (0x80 >> glyphX)) !== 0;
                if (!set && transparentCellBg) {
                    offset += 4;
                    continue;
                }

                writePixel(data, offset, set ? fgRgb : bgRgb);
                offset += 4;
            }
        }
    }
}

function writePixel(data, offset, rgb) {
    data[offset] = rgb[0];
    data[offset + 1] = rgb[1];
    data[offset + 2] = rgb[2];
    data[offset + 3] = 255;
}

function activePalette() {
    return exports.nova_palette_mode && (exports.nova_palette_mode() & 0x01) ? EGA_PALETTE : C64_PALETTE;
}

function currentVideoState() {
    const paletteMode = exports.nova_palette_mode ? exports.nova_palette_mode() & 0x01 : 0;
    const transparent = exports.nova_gfx_transparent_color ? exports.nova_gfx_transparent_color() & 0x0F : 0;
    const copperEnabled = exports.nova_copper_enabled ? exports.nova_copper_enabled() & 0x01 : 0;
    const copperCount = exports.nova_copper_event_count ? exports.nova_copper_event_count() >>> 0 : 0;
    return [
        regs[0x00] || 0,
        (regs[0x01] || 0) & 0x0F,
        (regs[0x0D] || 0) & 0x0F,
        paletteMode,
        transparent,
        copperEnabled,
        copperCount
    ].join(":");
}

function advanceCursorBlink() {
    cursorCounter++;
    if (cursorCounter >= 30) {
        cursorCounter = 0;
        cursorVisible = !cursorVisible;
        lastDirtyVersion = 0;
    }
}

function detectReadySignal() {
    const prompts = runtimeName === "forth"
        ? [[79, 75, 62]]
        : runtimeName === "logo"
            ? [[63, 32]]
            : [[82, 101, 97, 100, 121]];
    for (let row = 0; row < TEXT_ROWS; row++) {
        for (const prompt of prompts) {
            for (let col = 0; col <= TEXT_COLS - prompt.length; col++) {
                let ok = true;
                for (let i = 0; i < prompt.length; i++) {
                    if (screen[row * TEXT_COLS + col + i] !== prompt[i]) {
                        ok = false;
                        break;
                    }
                }
                if (ok) return true;
            }
        }
    }
    return false;
}

function queueKey(key, shift, control, meta) {
    if (!exports || meta) return;
    const code = keyToByte(key, control);
    if (code !== null) {
        exports.nova_queue_byte(code);
    }
}

function queueText(text) {
    if (!exports || !text) return;
    for (const ch of text) {
        if (ch === "\n") {
            exports.nova_queue_byte(13);
        } else {
            const code = ch.charCodeAt(0);
            if (code >= 0 && code <= 255) {
                exports.nova_queue_byte(code);
            }
        }
    }
}

function keyToByte(key, control) {
    if (control) return null;
    switch (key) {
        case "Enter": return 13;
        case "Backspace": return 8;
        case "Tab": return 9;
        case "Escape": return 3;
        default:
            if (key.length === 1) {
                const code = key.charCodeAt(0);
                return code <= 255 ? code : null;
            }
            return null;
    }
}

function reportStats() {
    const now = performance.now();
    const elapsedMs = Math.max(1, now - lastStatsTime);
    const elapsedSec = elapsedMs / 1000;
    postMessage({
        type: "metrics",
        metrics: {
            cpuHz: Math.round(stats.cpuCycles / elapsedSec),
            cpuLoops: stats.cpuLoops,
            cpuMsAvg: stats.cpuLoops ? stats.cpuMs / stats.cpuLoops : 0,
            cpuMsMax: stats.maxCpuMs,
            timingHz: stats.timingHz || 0,
            renderHz: Math.round(stats.renderFrames / elapsedSec),
            renderMsAvg: stats.renderFrames ? stats.renderMs / stats.renderFrames : 0,
            renderMsMax: stats.maxRenderMs,
            vblankHz: Math.round(stats.vblankFrames / elapsedSec),
            packetKBps: Math.round(stats.packetBytes / 1024 / elapsedSec),
            fullPackets: stats.packetType0,
            rowPackets: stats.packetType1,
            emptyPackets: stats.packetType2,
            activePackets: stats.packetType3,
            dirtyRows: stats.dirtyRows
        }
    });
    stats = createStats();
    lastStatsTime = now;
}

function stopStatsTimer() {
    if (statsTimer) {
        clearInterval(statsTimer);
        statsTimer = 0;
    }
}

function createStats() {
    return {
        cpuLoops: 0,
        cpuCycles: 0,
        cpuMs: 0,
        maxCpuMs: 0,
        timingHz: 0,
        renderFrames: 0,
        renderMs: 0,
        maxRenderMs: 0,
        vblankFrames: 0,
        packetBytes: 0,
        packetType0: 0,
        packetType1: 0,
        packetType2: 0,
        packetType3: 0,
        dirtyRows: 0
    };
}

function configureFromQuery(query) {
    const params = new URLSearchParams(query);
    renderHz = readNumberParam(params, "renderHz", 60, 1, 60);
    promptMs = readNumberParam(params, "promptMs", 33, 1, 100);
    programMs = readNumberParam(params, "programMs", 33, 1, 100);
    bootMs = readNumberParam(params, "bootMs", 33, 1, 100);
    promptCpuHz = readNumberParam(params, "promptHz", 12000000, 100000, 12000000);
    programCpuHz = readNumberParam(params, "programHz", 12000000, 100000, 12000000);
    bootCpuHz = readNumberParam(params, "bootHz", 12000000, 100000, 12000000);
    targetFrameMs = 1000 / renderHz;
    statsEnabled = readBoolParam(params, "stats", true);
    skipAutoboot = readBoolParam(params, "noAuto", false);
    runtimeName = normalizeRuntime(params.get("runtime") || runtimeName);
}

function readNumberParam(params, name, fallback, min, max) {
    const raw = params.get(name);
    if (!raw) return fallback;
    const value = Number(raw);
    if (!Number.isFinite(value)) return fallback;
    return Math.min(max, Math.max(min, value));
}

function readBoolParam(params, name, fallback) {
    const raw = params.get(name);
    if (raw === null) return fallback;
    return raw === "" || raw === "1" || raw.toLowerCase() === "true" || raw.toLowerCase() === "yes";
}

function normalizeRuntime(value) {
    const name = String(value || "basic").trim().toLowerCase();
    if (name === "novabasic" || name === "ehbasic") return "basic";
    if (name === "novalogo") return "logo";
    if (name === "novaforth") return "forth";
    return RUNTIMES[name] ? name : "basic";
}

function clampNumber(value, min, max, fallback) {
    const number = Number(value);
    if (!Number.isFinite(number)) return fallback;
    return Math.min(max, Math.max(min, Math.floor(number)));
}

function formatError(error) {
    if (!error) return "unknown error";
    return error.stack || error.message || String(error);
}
