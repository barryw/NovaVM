#![no_std]

#[cfg(test)]
extern crate std;

#[cfg(not(test))]
use core::panic::PanicInfo;
use core::ptr::{addr_of, addr_of_mut, null_mut};

const RAM_SIZE: usize = 65_536;
const ROM_BASE: u16 = 0xC000;
const ROM_SIZE: usize = 0x4000;
const VGC_BASE: u16 = 0xA000;
const AUTOBOOT_SKIP: usize = 0xB9F0;
const REG_ROMSWAP: u16 = 0xA03F;
const ROMSWAP_BASIC: u8 = 0x02;
const ROMSWAP_EXTENSION: u8 = 0x04;
const ROMSWAP_LOGO: u8 = 0x05;
const ROMSWAP_FORTH: u8 = 0x06;
const LIB_LOADER_BAND: usize = 0x0320;
const LIBCALL_LOAD_LIMIT: usize = 0x0100;
const SHELF_BASE: usize = 0x060000;
const SHELF_TAG: usize = 0x0418;
const SHELF_LRU: usize = 0x041C;
const HOST_EXT_XRAM: usize = 0x07C000;
const MODULE_SLOT_BYTES: usize = 0x4000;
const MODULE_SLOTS: usize = 4;
const MODULE_STORE_SLOTS: usize = 8;
const XRAM_SIZE: usize = 0x080000;
const MODULE_STORE_IDS: [u8; MODULE_STORE_SLOTS] = [0x01, 0x03, 0x02, 0x08, 0x04, 0x05, 0x06, 0x07];
const BOOT_MODULE_STORE_SLOTS: [usize; MODULE_SLOTS] = [0, 1, 2, 3];
const SCREEN_COLS: usize = 80;
const SCREEN_ROWS: usize = 50;
const SCREEN_SIZE: usize = SCREEN_COLS * SCREEN_ROWS;
const GFX_WIDTH: usize = 320;
const GFX_HEIGHT: usize = 200;
const GFX_SIZE: usize = GFX_WIDTH * GFX_HEIGHT;
const SPRITE_SHAPE_SIZE: usize = 128;
const SPRITE_SHAPE_COUNT: usize = 256;
const SPRITE_SHAPE_RAM_SIZE: usize = SPRITE_SHAPE_SIZE * SPRITE_SHAPE_COUNT;
const MAX_SPRITES: usize = 16;
const ROM_LOAD_LIMIT: usize = ROM_SIZE;

const REG_HELP: u16 = 0xA020;
const HELP_SEARCH_END: u16 = 0xA030;
const CMP_BASE: u16 = 0xA031;
const CMP_END: u16 = 0xA03E;
const SPRITE_REG_BASE: u16 = 0xA040;
const SPRITE_REG_END: u16 = 0xA0BF;
const SPRITE_REG_STRIDE: usize = 8;
const SPR_REG_X_LO: usize = 0;
const SPR_REG_X_HI: usize = 1;
const SPR_REG_Y_LO: usize = 2;
const SPR_REG_Y_HI: usize = 3;
const SPR_REG_SHAPE: usize = 4;
const SPR_REG_FLAGS: usize = 5;
const SPR_REG_PRIORITY: usize = 6;
const SPR_REG_TRANS_COLOR: usize = 7;
const SPR_FLAG_ENABLE: u8 = 0x80;
const VRAM_PLANE: u16 = 0xA0E0;
const VRAM_ADDR_L: u16 = 0xA0E1;
const VRAM_ADDR_H: u16 = 0xA0E2;
const VRAM_DATA: u16 = 0xA0E3;
const VRAM_CTRL: u16 = 0xA0E4;
const VRAM_REG_END: u16 = 0xA0E4;
const VRAM_CTRL_AUTO_INC: u8 = 0x01;
const DISPLAY_DIM: u16 = 0xA0E5;
const REG_TEXT_FLAGS: u16 = 0xA0E6;
const REG_TEXT_REVERSE_ATTR: u16 = 0xA0E7;
const REG_GFX_TRANSPARENT_COLOR: u16 = 0xA0E8;
const REG_PALETTE_MODE: u16 = 0xA0E9;
const REG_SCROLL_CTL: u16 = 0xA0EA;
const REG_COL_ST_HI: u16 = 0xA0EB;
const REG_COL_BG_HI: u16 = 0xA0EC;
const REG_TEXT_TOP_ROW: u16 = 0xA0ED;
const REG_TEXT_SCROLL_START: u16 = 0xA0EE;
const REG_TEXT_SCROLL_ROWS: u16 = 0xA0EF;
const VGC_IRQ_BASE: u16 = 0xA0F0;
const VGC_IRQ_END: u16 = 0xA0FF;
const REG_IRQ_ENABLE: u16 = 0xA0F0;
const REG_IRQ_STATUS: u16 = 0xA0F1;
const REG_IRQ_FORCE: u16 = 0xA0F2;
const REG_IRQ_VALID: u16 = 0xA0F3;
const IRQ_VALID_MASK: u8 = 0x7F;
const IRQ_VBLANK: u8 = 0x01;
#[cfg(test)]
const IRQ_COPPER0: u8 = 0x02;

const REG_MODE: u16 = 0xA000;
const REG_BG: u16 = 0xA001;
const REG_FG: u16 = 0xA002;
const REG_CURSOR_X: u16 = 0xA003;
const REG_CURSOR_Y: u16 = 0xA004;
const REG_SCROLL_X: u16 = 0xA005;
const REG_SCROLL_Y: u16 = 0xA006;
const REG_FONT: u16 = 0xA007;
const REG_STATUS: u16 = 0xA008;
const REG_SPRITE_COUNT: u16 = 0xA009;
const REG_CURSOR_ENABLE: u16 = 0xA00A;
const REG_COL_ST: u16 = 0xA00B;
const REG_COL_BG: u16 = 0xA00C;
const REG_BORDER: u16 = 0xA00D;
const REG_CHAR_OUT: u16 = 0xA00E;
const REG_CHAR_IN: u16 = 0xA00F;
const REG_CMD: u16 = 0xA010;
const REG_P14: u16 = 0xA01F;
const CMD_PLOT: u8 = 0x01;
const CMD_UNPLOT: u8 = 0x02;
const CMD_LINE: u8 = 0x03;
const CMD_CIRCLE: u8 = 0x04;
const CMD_RECT: u8 = 0x05;
const CMD_FILL: u8 = 0x06;
const CMD_GCLS: u8 = 0x07;
const CMD_GCOLOR: u8 = 0x08;
const CMD_PAINT: u8 = 0x09;
const CMD_SPR_DEF: u8 = 0x10;
const CMD_SPR_ROW: u8 = 0x11;
const CMD_SPR_CLR: u8 = 0x12;
const CMD_SPR_COPY: u8 = 0x13;
const CMD_SPR_POS: u8 = 0x14;
const CMD_SPR_ENA: u8 = 0x15;
const CMD_SPR_DIS: u8 = 0x16;
const CMD_SPR_FLIP: u8 = 0x17;
const CMD_SPR_PRI: u8 = 0x18;
const CMD_MEM_READ: u8 = 0x19;
const CMD_MEM_WRITE: u8 = 0x1A;
const CMD_COPPER_ADD: u8 = 0x1B;
const CMD_COPPER_CLEAR: u8 = 0x1C;
const CMD_COPPER_ENABLE: u8 = 0x1D;
const CMD_COPPER_DISABLE: u8 = 0x1E;
const CMD_SYS_RESET: u8 = 0x1F;
const CMD_COPPER_LIST: u8 = 0x20;
const CMD_COPPER_USE: u8 = 0x21;
const CMD_COPPER_LIST_END: u8 = 0x22;
const COPPER_REG_IRQ: u8 = 0xFE;
const COPPER_LIST_COUNT: usize = 128;
const MAX_COPPER_EVENTS_PER_LIST: usize = 256;
const MEM_SPACE_SCREEN: u8 = 0x01;
const MEM_SPACE_COLOR: u8 = 0x02;
const MEM_SPACE_GFX: u8 = 0x03;
const MEM_SPACE_SPRITE: u8 = 0x04;
const MEM_SPACE_TEXT_ATTR: u8 = 0x07;
const NIC_BASE: u16 = 0xA100;
const NIC_END: u16 = 0xA13F;
const NIC_STATUS: u16 = 0xA101;
const NIC_IRQ_STATUS: u16 = 0xA104;
const NIC_DMA_STATUS: u16 = 0xA114;
const NIC_SLOT_STATUS0: u16 = 0xA118;
const NIC_SLOT_STATUS3: u16 = 0xA11B;
const NIC_STATUS_READY: u8 = 0x01;
const NIC_SLOT_SEND_READY: u8 = 0x04;
const NIC_DMA_STATUS_TX_READY: u8 = 0x01;
const WTS_BASE: u16 = 0xA140;
const WTS_END: u16 = 0xA1FF;
const WTS_MASTER_VOLUME: u16 = 0xA182;
const WTS_SOUNDFONT_STATUS: u16 = 0xA183;
const WTS_EVENT_STATUS: u16 = 0xA1E0;
const SCREEN_WIN_BASE: u16 = 0xA200;
const SCREEN_WIN_END: u16 = 0xB19F;
const SCREEN_WIN_PLANE_SEL: u16 = 0xB1A0;
const SCREEN_WIN_PLANE_CHAR: u8 = 0x00;
const SCREEN_WIN_PLANE_COLOR: u8 = 0x01;
const SCREEN_WIN_PLANE_ATTR: u8 = 0x02;
const FIO_BASE: u16 = 0xB9A0;
const FIO_END: u16 = 0xB9EF;
const FIO_CMD: u16 = 0xB9A0;
const FIO_STATUS: u16 = 0xB9A1;
const FIO_ERRCODE: u16 = 0xB9A2;
const FIO_SRC_L: u16 = 0xB9A4;
const FIO_SRC_H: u16 = 0xB9A5;
const FIO_END_L: u16 = 0xB9A6;
const FIO_END_H: u16 = 0xB9A7;
const FIO_CMD_SAVE: u8 = 0x01;
const FIO_CMD_LOAD: u8 = 0x02;
const FIO_CMD_DIR_OPEN: u8 = 0x03;
const FIO_CMD_DIR_READ: u8 = 0x04;
const FIO_CMD_DELETE: u8 = 0x05;
const FIO_CMD_GSAVE: u8 = 0x06;
const FIO_CMD_GLOAD: u8 = 0x07;
const FIO_CMD_XLOAD: u8 = 0x18;
const FIO_CMD_XSAVE: u8 = 0x19;
const FIO_CMD_CD: u8 = 0x20;
const FIO_CMD_MKDIR: u8 = 0x21;
const FIO_CMD_RMDIR: u8 = 0x22;
const FIO_CMD_MOUNT: u8 = 0x24;
const FIO_CMD_UNMOUNT: u8 = 0x25;
const FIO_CMD_PWD: u8 = 0x26;
const FIO_CMD_CLEARERR: u8 = 0x27;
const FIO_CMD_LOAD_RUNTIME: u8 = 0x28;
const FIO_CMD_XPAGE: u8 = 0x29;
const FIO_CMD_RNG: u8 = 0x2A;
const FIO_CMD_NVGLOAD: u8 = 0x2B;
const FIO_CMD_LOAD_MODULE: u8 = 0x2C;
const FIO_CMD_FOPEN: u8 = 0x2D;
const FIO_CMD_FRENAME: u8 = 0x39;
const FIO_CMD_DEVSTATUS: u8 = 0x3A;
const FIO_STATUS_OK: u8 = 0x02;
const FIO_STATUS_ERROR: u8 = 0x03;
const XMC_BASE: u16 = 0xBA00;
const XMC_END: u16 = 0xBA3F;
const XMC_WINDOW_BASE: u16 = 0xBC00;
const XMC_WINDOW_END: u16 = 0xBFFF;
const XMC_CMD: u16 = 0xBA00;
const XMC_STATUS: u16 = 0xBA01;
const XMC_ERRCODE: u16 = 0xBA02;
const XMC_XAL: u16 = 0xBA04;
const XMC_XAM: u16 = 0xBA05;
const XMC_XAH: u16 = 0xBA06;
const XMC_RAML: u16 = 0xBA07;
const XMC_RAMH: u16 = 0xBA08;
const XMC_LENL: u16 = 0xBA09;
const XMC_LENH: u16 = 0xBA0A;
const XMC_DATA: u16 = 0xBA0B;
const XMC_BANKS: u16 = 0xBA0D;
const XMC_WINCTL: u16 = 0xBA16;
const XMC_WIN0AL: u16 = 0xBA18;
const XMC_CMD_GET: u8 = 0x01;
const XMC_CMD_PUT: u8 = 0x02;
const XMC_CMD_STASH: u8 = 0x03;
const XMC_CMD_FETCH: u8 = 0x04;
const XMC_CMD_FILL: u8 = 0x05;
const XMC_CMD_STATS: u8 = 0x07;
const XMC_STATUS_OK: u8 = 0x02;
const XMC_STATUS_ERROR: u8 = 0x03;
const XMC_ERR_NONE: u8 = 0x00;
const XMC_ERR_RANGE: u8 = 0x01;
const XMC_ERR_BADARGS: u8 = 0x02;
const TIMER_BASE: u16 = 0xBA40;
const TIMER_END: u16 = 0xBA4F;
const TIMER_CTRL: u16 = 0xBA40;
const TIMER_STATUS: u16 = 0xBA41;
const TIMER_DIV_L: u16 = 0xBA42;
const TIMER_DIV_H: u16 = 0xBA43;
const TIMER_TICK_QUANTUM_CYCLES: i32 = 100;
const MUSIC_STATUS: u16 = 0xBA50;
const MUSIC_TOTAL_H: u16 = 0xBA62;
const DMA_BASE: u16 = 0xBA63;
const DMA_END: u16 = 0xBA75;
const DMA_CMD: u16 = 0xBA63;
const DMA_STATUS: u16 = 0xBA64;
const DMA_ERRCODE: u16 = 0xBA65;
const DMA_SRC_SPACE: u16 = 0xBA66;
const DMA_DST_SPACE: u16 = 0xBA67;
const DMA_SRC_L: u16 = 0xBA68;
const DMA_DST_L: u16 = 0xBA6B;
const DMA_LEN_L: u16 = 0xBA6E;
const DMA_MODE: u16 = 0xBA71;
const DMA_FILL_VALUE: u16 = 0xBA72;
const DMA_COUNT_L: u16 = 0xBA73;
const DMA_CMD_START: u8 = 0x01;
const DMA_STATUS_IDLE: u8 = 0x00;
const DMA_STATUS_OK: u8 = 0x02;
const DMA_STATUS_ERROR: u8 = 0x03;
const DMA_ERR_NONE: u8 = 0x00;
const DMA_ERR_BAD_CMD: u8 = 0x01;
const DMA_ERR_RANGE: u8 = 0x03;
const DMA_ERR_BAD_ARGS: u8 = 0x04;
const DMA_ERR_WRITE_PROT: u8 = 0x05;
const DMA_MODE_FILL: u8 = 0x01;
const DMA_SPACE_CPU_RAM: u8 = 0x00;
const DMA_SPACE_VGC_CHAR: u8 = 0x01;
const DMA_SPACE_VGC_COLOR: u8 = 0x02;
const DMA_SPACE_VGC_GFX: u8 = 0x03;
const DMA_SPACE_VGC_SPRITE: u8 = 0x04;
const DMA_SPACE_XRAM: u8 = 0x05;
const DMA_SPACE_VGC_TEXT_ATTR: u8 = 0x07;
const PGD_CMD: u16 = 0xBA76;
const PGD_STATUS: u16 = 0xBA77;
const PGD_SRC_L: u16 = 0xBA78;
const PGD_SRC_M: u16 = 0xBA79;
const PGD_SRC_H: u16 = 0xBA7A;
const PGD_WORDS_L: u16 = 0xBA7B;
const PGD_WORDS_H: u16 = 0xBA7C;
const PGD_START: u8 = 0x01;
const PGD_STATUS_DONE: u8 = 0x02;
const BLT_BASE: u16 = 0xBA83;
const BLT_END: u16 = 0xBA9B;
const BLT_ROTATE_ANGLE: u16 = 0xBAA2;
const BLT_CMD: u16 = 0xBA83;
const BLT_STATUS: u16 = 0xBA84;
const BLT_ERRCODE: u16 = 0xBA85;
const BLT_SRC_SPACE: u16 = 0xBA86;
const BLT_DST_SPACE: u16 = 0xBA87;
const BLT_SRC_L: u16 = 0xBA88;
const BLT_DST_L: u16 = 0xBA8B;
const BLT_WIDTH_L: u16 = 0xBA8E;
const BLT_HEIGHT_L: u16 = 0xBA90;
const BLT_SRC_STRIDE_L: u16 = 0xBA92;
const BLT_DST_STRIDE_L: u16 = 0xBA94;
const BLT_MODE: u16 = 0xBA96;
const BLT_FILL_VALUE: u16 = 0xBA97;
const BLT_COLOR_KEY: u16 = 0xBA98;
const BLT_COUNT_L: u16 = 0xBA99;
const BLT_CMD_START: u8 = 0x01;
const BLT_STATUS_IDLE: u8 = 0x00;
const BLT_STATUS_OK: u8 = 0x02;
const BLT_STATUS_ERROR: u8 = 0x03;
const BLT_MODE_FILL: u8 = 0x01;
const BLT_MODE_COLOR_KEY: u8 = 0x02;
const BLT_MODE_ROTATE: u8 = 0x04;
const BOARD_INPUT_BASE: u16 = 0xBA9C;
const BOARD_INPUT_END: u16 = 0xBAA1;
const BOARD_INPUT_BUTTONS: u16 = 0xBA9C;
const BOARD_INPUT_SWITCHES: u16 = 0xBA9D;
const BOARD_INPUT_IRQ_ENABLE: u16 = 0xBA9E;
const BOARD_INPUT_IRQ_STATUS: u16 = 0xBA9F;
const BOARD_INPUT_BUTTON_CHANGES: u16 = 0xBAA0;
const BOARD_INPUT_SWITCH_CHANGES: u16 = 0xBAA1;
const BOARD_INPUT_IRQ_MASK: u8 = 0x03;
const USB_HID_DIAG_BASE: u16 = 0xBAA3;
const USB_HID_DIAG_END: u16 = 0xBAAF;
const META_BASE: u16 = 0xBAB0;
const META_END: u16 = 0xBB1F;
const MATH_BASE: u16 = 0xBB20;
const MATH_END: u16 = 0xBB4F;
const MATH_MUL16_B_HI: u16 = 0xBB23;
const MATH_MULFX_B_HI: u16 = 0xBB27;
const MATH_DIV_D_HI: u16 = 0xBB2D;
const MATH_SINCOS_ANGLE: u16 = 0xBB2E;
const MATH_ATAN_DX_HI: u16 = 0xBB32;
const MATH_DIST_DY_HI: u16 = 0xBB36;
const MATH_RNG: u16 = 0xBB37;
const MATH_RES0: u16 = 0xBB38;
const MATH_CAPS0: u16 = 0xBB3C;
const MATH_CAPS1: u16 = 0xBB3D;
const MATH_STATUS: u16 = 0xBB3E;
const MATH_VERSION: u16 = 0xBB3F;
const MATH_VEC_OP: u16 = 0xBB4E;
const MATH_STATUS_OK: u8 = 0x00;
const MATH_STATUS_DIV_ZERO: u8 = 0x01;
const MATH_STATUS_OVERFLOW: u8 = 0x02;
const MATH_STATUS_UNIMPL: u8 = 0x80;
const MATH_VERSION_CURRENT: u8 = 0x02;
const MATH_CAPS0_VALUE: u8 = 0x7F;
const MATH_CAPS1_VALUE: u8 = 0x1F;
const MUSIC_META_SOUNDFONT: u16 = 0xBB8E;
const MUSIC_META_SOUNDFONT_END: u16 = 0xBBCD;
const SIN_QUARTER_MAGNITUDES: [u8; 65] = [
    0, 3, 6, 9, 13, 16, 19, 22, 25, 28, 31, 34, 37, 40, 43, 46, 49, 52, 55, 58, 60, 63, 66, 68, 71,
    74, 76, 79, 81, 84, 86, 88, 91, 93, 95, 97, 99, 101, 103, 105, 106, 108, 110, 111, 113, 114,
    116, 117, 118, 119, 121, 122, 122, 123, 124, 125, 126, 126, 127, 127, 127, 128, 128, 128, 128,
];

const C: u8 = 0x01;
const Z: u8 = 0x02;
const I: u8 = 0x04;
const D: u8 = 0x08;
const B: u8 = 0x10;
const U: u8 = 0x20;
const V: u8 = 0x40;
const N: u8 = 0x80;

#[cfg(not(test))]
#[panic_handler]
fn panic(_: &PanicInfo) -> ! {
    loop {}
}

static mut ROM_BUFFER: [u8; ROM_LOAD_LIMIT] = [0; ROM_LOAD_LIMIT];
static mut EXT_BUFFER: [u8; ROM_LOAD_LIMIT] = [0; ROM_LOAD_LIMIT];
static mut LIBCALL_BUFFER: [u8; LIBCALL_LOAD_LIMIT] = [0; LIBCALL_LOAD_LIMIT];
static mut MODULE_BUFFERS: [[u8; MODULE_SLOT_BYTES]; MODULE_STORE_SLOTS] =
    [[0; MODULE_SLOT_BYTES]; MODULE_STORE_SLOTS];
static mut VM: Vm = Vm::new();

#[repr(C)]
#[derive(Clone, Copy)]
pub struct CopperEvent {
    position: u16,
    register_index: u8,
    value: u8,
}

const EMPTY_COPPER_EVENT: CopperEvent = CopperEvent {
    position: 0,
    register_index: 0,
    value: 0,
};

unsafe fn vm_ref() -> &'static Vm {
    &*addr_of!(VM)
}

unsafe fn vm_mut() -> &'static mut Vm {
    &mut *addr_of_mut!(VM)
}

#[no_mangle]
pub extern "C" fn nova_rom_ptr() -> *mut u8 {
    addr_of_mut!(ROM_BUFFER).cast::<u8>()
}

#[no_mangle]
pub extern "C" fn nova_ext_ptr() -> *mut u8 {
    addr_of_mut!(EXT_BUFFER).cast::<u8>()
}

#[no_mangle]
pub extern "C" fn nova_libcall_ptr() -> *mut u8 {
    addr_of_mut!(LIBCALL_BUFFER).cast::<u8>()
}

#[no_mangle]
pub extern "C" fn nova_module_ptr(slot: usize) -> *mut u8 {
    if slot >= MODULE_STORE_SLOTS {
        return null_mut();
    }
    unsafe {
        addr_of_mut!(MODULE_BUFFERS)
            .cast::<u8>()
            .add(slot * MODULE_SLOT_BYTES)
    }
}

#[no_mangle]
pub extern "C" fn nova_init(rom_len: usize, skip_autoboot: u32) {
    unsafe {
        let vm = vm_mut();
        let len = min_usize(rom_len, ROM_LOAD_LIMIT);
        vm.reset();
        core::ptr::copy_nonoverlapping(
            addr_of!(ROM_BUFFER).cast::<u8>(),
            vm.basic_rom.as_mut_ptr(),
            len,
        );
        vm.map_basic_rom();
        core::ptr::copy_nonoverlapping(
            addr_of!(LIBCALL_BUFFER).cast::<u8>(),
            vm.ram.as_mut_ptr().add(LIB_LOADER_BAND),
            LIBCALL_LOAD_LIMIT,
        );
        core::ptr::copy_nonoverlapping(
            addr_of!(EXT_BUFFER).cast::<u8>(),
            vm.xram.as_mut_ptr().add(HOST_EXT_XRAM),
            ROM_LOAD_LIMIT,
        );
        vm.stage_boot_modules();
        if skip_autoboot != 0 {
            vm.ram[AUTOBOOT_SKIP] = 0xFF;
        }
        vm.pc = vm.read_word(0xFFFC);
        vm.p = I | U;
    }
}

#[no_mangle]
pub extern "C" fn nova_load_primary_rom(rom_len: usize) {
    unsafe {
        let vm = vm_mut();
        let len = min_usize(rom_len, ROM_SIZE);
        vm.basic_rom.fill(0);
        core::ptr::copy_nonoverlapping(
            addr_of!(ROM_BUFFER).cast::<u8>(),
            vm.basic_rom.as_mut_ptr(),
            len,
        );
        vm.map_basic_rom();
    }
}

#[no_mangle]
pub extern "C" fn nova_run_cycles(target_cycles: u32) -> u32 {
    unsafe { vm_mut().run_cycles(target_cycles) }
}

#[no_mangle]
pub extern "C" fn nova_queue_byte(value: u8) {
    unsafe { vm_mut().queue_byte(value) }
}

#[no_mangle]
pub extern "C" fn nova_vblank() {
    unsafe { vm_mut().vblank() }
}

#[no_mangle]
pub extern "C" fn nova_ram_ptr() -> *mut u8 {
    unsafe { vm_mut().ram.as_mut_ptr() }
}

#[no_mangle]
pub extern "C" fn nova_xram_ptr() -> *mut u8 {
    unsafe { vm_mut().xram.as_mut_ptr() }
}

#[no_mangle]
pub extern "C" fn nova_screen_ptr() -> *const u8 {
    unsafe { vm_ref().screen.as_ptr() }
}

#[no_mangle]
pub extern "C" fn nova_color_ptr() -> *const u8 {
    unsafe { vm_ref().color.as_ptr() }
}

#[no_mangle]
pub extern "C" fn nova_attr_ptr() -> *const u8 {
    unsafe { vm_ref().attr.as_ptr() }
}

#[no_mangle]
pub extern "C" fn nova_gfx_ptr() -> *const u8 {
    unsafe { vm_ref().gfx.as_ptr() }
}

#[no_mangle]
pub extern "C" fn nova_sprite_shape_ptr() -> *const u8 {
    unsafe { vm_ref().sprite_shapes.as_ptr() }
}

#[no_mangle]
pub extern "C" fn nova_sprite_regs_ptr() -> *const u8 {
    unsafe { vm_ref().sprite_regs.as_ptr() }
}

#[no_mangle]
pub extern "C" fn nova_regs_ptr() -> *const u8 {
    unsafe { vm_ref().vgc_regs.as_ptr() }
}

#[no_mangle]
pub extern "C" fn nova_gfx_transparent_color() -> u32 {
    unsafe { vm_ref().gfx_transparent_color as u32 }
}

#[no_mangle]
pub extern "C" fn nova_palette_mode() -> u32 {
    unsafe { vm_ref().palette_mode as u32 }
}

#[no_mangle]
pub extern "C" fn nova_copper_enabled() -> u32 {
    unsafe {
        if vm_ref().copper_enabled {
            1
        } else {
            0
        }
    }
}

#[no_mangle]
pub extern "C" fn nova_copper_events_ptr() -> *const CopperEvent {
    unsafe {
        let vm = vm_ref();
        vm.copper_events[vm.copper_active_list].as_ptr()
    }
}

#[no_mangle]
pub extern "C" fn nova_copper_event_count() -> u32 {
    unsafe {
        let vm = vm_ref();
        vm.copper_counts[vm.copper_active_list] as u32
    }
}

#[no_mangle]
pub extern "C" fn nova_raise_copper_irq(source_mask: u32) {
    unsafe { vm_mut().raise_vgc_irq(source_mask as u8) }
}

#[no_mangle]
pub extern "C" fn nova_dirty_version() -> u32 {
    unsafe { vm_ref().dirty_version }
}

#[no_mangle]
pub extern "C" fn nova_mark_dirty() {
    unsafe { vm_mut().dirty() }
}

#[no_mangle]
pub extern "C" fn nova_fio_pending_cmd() -> u32 {
    unsafe { vm_ref().pending_fio_cmd as u32 }
}

#[no_mangle]
pub extern "C" fn nova_fio_complete(status: u32, err: u32) {
    unsafe {
        let vm = vm_mut();
        vm.pending_fio_cmd = 0;
        vm.ram[FIO_STATUS as usize] = status as u8;
        vm.ram[FIO_ERRCODE as usize] = err as u8;
        vm.ram[FIO_CMD as usize] = 0;
        vm.dirty();
    }
}

#[no_mangle]
pub extern "C" fn nova_pc() -> u32 {
    unsafe { vm_ref().pc as u32 }
}

#[no_mangle]
pub extern "C" fn nova_trap_code() -> u32 {
    unsafe {
        let vm = vm_ref();
        if !vm.trapped {
            0
        } else {
            ((vm.trap_pc as u32) << 8) | vm.trap_opcode as u32
        }
    }
}

struct Vm {
    ram: [u8; RAM_SIZE],
    basic_rom: [u8; ROM_SIZE],
    ext_bank: [u8; ROM_SIZE],
    xram: [u8; XRAM_SIZE],
    screen: [u8; SCREEN_SIZE],
    color: [u8; SCREEN_SIZE],
    attr: [u8; SCREEN_SIZE],
    gfx: [u8; GFX_SIZE],
    sprite_shapes: [u8; SPRITE_SHAPE_RAM_SIZE],
    sprite_regs: [u8; MAX_SPRITES * SPRITE_REG_STRIDE],
    vgc_regs: [u8; 16],
    cmd_regs: [u8; 16],
    xmc_regs: [u8; 64],
    timer_regs: [u8; 16],
    dma_regs: [u8; 19],
    blt_regs: [u8; 26],
    board_regs: [u8; 6],
    math_regs: [u8; 48],
    nic_regs: [u8; 64],
    wts_regs: [u8; 192],
    gfx_draw_color: u8,
    gfx_transparent_color: u8,
    palette_mode: u8,
    display_dim: u8,
    text_flags: u8,
    text_reverse_attr: u8,
    scroll_ctl: u8,
    text_top_row: u8,
    text_scroll_start: u8,
    text_scroll_rows: u8,
    copper_events: [[CopperEvent; MAX_COPPER_EVENTS_PER_LIST]; COPPER_LIST_COUNT],
    copper_counts: [u16; COPPER_LIST_COUNT],
    copper_target_list: usize,
    copper_active_list: usize,
    copper_pending_list: usize,
    copper_enabled: bool,
    irq_enable: u8,
    irq_status: u8,
    collision_sprite: u16,
    collision_background: u16,
    vram_plane: u8,
    vram_addr: u16,
    vram_ctrl: u8,
    vram_read_latch: u8,
    screen_win_plane: u8,
    fill_queue: [u16; GFX_SIZE],
    input: [u8; 256],
    input_head: u8,
    input_tail: u8,
    dirty_version: u32,
    pgd_src: usize,
    pgd_words: usize,
    timer_counter: i32,
    timer_cycle_accumulator: i32,
    math_rng_state: u32,
    math_rng_seed_index: u8,
    ext_bank_valid: bool,
    pending_fio_cmd: u8,
    trapped: bool,
    trap_opcode: u8,
    trap_pc: u16,
    flat_bus: bool,
    cmos: bool,
    irq_waiting: bool,
    nmi_waiting: bool,
    a: u8,
    x: u8,
    y: u8,
    sp: u8,
    pc: u16,
    p: u8,
}

impl Vm {
    const fn new() -> Self {
        Self {
            ram: [0; RAM_SIZE],
            basic_rom: [0; ROM_SIZE],
            ext_bank: [0; ROM_SIZE],
            xram: [0; XRAM_SIZE],
            screen: [0x20; SCREEN_SIZE],
            color: [0x0F; SCREEN_SIZE],
            attr: [0; SCREEN_SIZE],
            gfx: [0; GFX_SIZE],
            sprite_shapes: [0; SPRITE_SHAPE_RAM_SIZE],
            sprite_regs: [0; MAX_SPRITES * SPRITE_REG_STRIDE],
            vgc_regs: [0; 16],
            cmd_regs: [0; 16],
            xmc_regs: [0; 64],
            timer_regs: [0; 16],
            dma_regs: [0; 19],
            blt_regs: [0; 26],
            board_regs: [0; 6],
            math_regs: [0; 48],
            nic_regs: [0; 64],
            wts_regs: [0; 192],
            gfx_draw_color: 1,
            gfx_transparent_color: 0,
            palette_mode: 0,
            display_dim: 15,
            text_flags: 0,
            text_reverse_attr: 0xF0,
            scroll_ctl: 0x06,
            text_top_row: 0,
            text_scroll_start: 0,
            text_scroll_rows: SCREEN_ROWS as u8,
            copper_events: [[EMPTY_COPPER_EVENT; MAX_COPPER_EVENTS_PER_LIST]; COPPER_LIST_COUNT],
            copper_counts: [0; COPPER_LIST_COUNT],
            copper_target_list: 0,
            copper_active_list: 0,
            copper_pending_list: 0,
            copper_enabled: false,
            irq_enable: 0,
            irq_status: 0,
            collision_sprite: 0,
            collision_background: 0,
            vram_plane: MEM_SPACE_SCREEN,
            vram_addr: 0,
            vram_ctrl: VRAM_CTRL_AUTO_INC,
            vram_read_latch: 0,
            screen_win_plane: SCREEN_WIN_PLANE_CHAR,
            fill_queue: [0; GFX_SIZE],
            input: [0; 256],
            input_head: 0,
            input_tail: 0,
            dirty_version: 1,
            pgd_src: 0,
            pgd_words: 0,
            timer_counter: 0,
            timer_cycle_accumulator: 0,
            math_rng_state: 0xDEADBEEF,
            math_rng_seed_index: 0,
            ext_bank_valid: false,
            pending_fio_cmd: 0,
            trapped: false,
            trap_opcode: 0,
            trap_pc: 0,
            flat_bus: false,
            cmos: true,
            irq_waiting: false,
            nmi_waiting: false,
            a: 0,
            x: 0,
            y: 0,
            sp: 0,
            pc: 0,
            p: I | U,
        }
    }

    fn reset(&mut self) {
        self.ram.fill(0);
        self.basic_rom.fill(0);
        self.ext_bank.fill(0);
        self.xram.fill(0);
        self.screen.fill(0x20);
        self.color.fill(0x0F);
        self.attr.fill(0);
        self.gfx.fill(0);
        self.sprite_shapes.fill(0);
        self.sprite_regs.fill(0);
        self.vgc_regs.fill(0);
        self.cmd_regs.fill(0);
        self.xmc_regs.fill(0);
        self.timer_regs.fill(0);
        self.dma_regs.fill(0);
        self.blt_regs.fill(0);
        self.board_regs.fill(0);
        self.math_regs.fill(0);
        self.nic_regs.fill(0);
        self.wts_regs.fill(0);
        self.xmc_regs[(XMC_BANKS - XMC_BASE) as usize] = (XRAM_SIZE / 0x10000) as u8;
        self.xmc_regs[(XMC_WINCTL - XMC_BASE) as usize] = 0x0F;
        self.gfx_draw_color = 1;
        self.gfx_transparent_color = 0;
        self.palette_mode = 0;
        self.display_dim = 15;
        self.text_flags = 0;
        self.text_reverse_attr = 0xF0;
        self.scroll_ctl = 0x06;
        self.text_top_row = 0;
        self.text_scroll_start = 0;
        self.text_scroll_rows = SCREEN_ROWS as u8;
        self.copper_events = [[EMPTY_COPPER_EVENT; MAX_COPPER_EVENTS_PER_LIST]; COPPER_LIST_COUNT];
        self.copper_counts = [0; COPPER_LIST_COUNT];
        self.copper_target_list = 0;
        self.copper_active_list = 0;
        self.copper_pending_list = 0;
        self.copper_enabled = false;
        self.irq_enable = 0;
        self.irq_status = 0;
        self.collision_sprite = 0;
        self.collision_background = 0;
        self.vram_plane = MEM_SPACE_SCREEN;
        self.vram_addr = 0;
        self.vram_ctrl = VRAM_CTRL_AUTO_INC;
        self.vram_read_latch = 0;
        self.screen_win_plane = SCREEN_WIN_PLANE_CHAR;
        self.vgc_regs[(REG_FG - VGC_BASE) as usize] = 15;
        self.vgc_regs[(REG_BG - VGC_BASE) as usize] = 0;
        self.vgc_regs[(REG_BORDER - VGC_BASE) as usize] = 11;
        self.timer_regs[(TIMER_STATUS - TIMER_BASE) as usize] = 0;
        self.dma_regs[(DMA_STATUS - DMA_BASE) as usize] = DMA_STATUS_IDLE;
        self.dma_regs[(DMA_ERRCODE - DMA_BASE) as usize] = DMA_ERR_NONE;
        self.blt_regs[(BLT_STATUS - BLT_BASE) as usize] = BLT_STATUS_IDLE;
        self.blt_regs[(BLT_ERRCODE - BLT_BASE) as usize] = DMA_ERR_NONE;
        self.wts_regs[(WTS_MASTER_VOLUME - WTS_BASE) as usize] = 0xFF;
        self.wts_regs[(WTS_EVENT_STATUS - WTS_BASE) as usize] = 0x05;
        for i in 0..MAX_SPRITES {
            let base = i * SPRITE_REG_STRIDE;
            self.sprite_regs[base + SPR_REG_SHAPE] = i as u8;
            self.sprite_regs[base + SPR_REG_PRIORITY] = 2;
        }
        self.input_head = 0;
        self.input_tail = 0;
        self.input.fill(0);
        self.fill_queue.fill(0);
        self.dirty();
        self.pgd_src = 0;
        self.pgd_words = 0;
        self.timer_counter = 0;
        self.timer_cycle_accumulator = 0;
        self.math_rng_state = 0xDEADBEEF;
        self.math_rng_seed_index = 0;
        self.ext_bank_valid = false;
        self.pending_fio_cmd = 0;
        self.trapped = false;
        self.trap_opcode = 0;
        self.trap_pc = 0;
        self.flat_bus = false;
        self.cmos = true;
        self.irq_waiting = false;
        self.nmi_waiting = false;
        self.a = 0;
        self.x = 0;
        self.y = 0;
        self.sp = 0;
        self.pc = 0;
        self.p = I | U;
    }

    fn run_cycles(&mut self, target_cycles: u32) -> u32 {
        let mut executed = 0;
        while executed < target_cycles && !self.trapped && self.pending_fio_cmd == 0 {
            let cycles = self.step() as u32;
            executed += cycles;
            self.advance_hardware(cycles);
        }
        executed
    }

    fn map_basic_rom(&mut self) {
        self.ram[ROM_BASE as usize..ROM_BASE as usize + ROM_SIZE].copy_from_slice(&self.basic_rom);
    }

    fn map_extension_rom(&mut self) {
        if self.ext_bank_valid {
            self.ram[ROM_BASE as usize..ROM_BASE as usize + ROM_SIZE]
                .copy_from_slice(&self.ext_bank);
        }
    }

    fn stage_boot_modules(&mut self) {
        for slot in 0..MODULE_SLOTS {
            self.ram[SHELF_LRU + slot] = slot as u8;
            let store_slot = BOOT_MODULE_STORE_SLOTS[slot];
            self.ram[SHELF_TAG + slot] = MODULE_STORE_IDS[store_slot];
            let dest = SHELF_BASE + slot * MODULE_SLOT_BYTES;
            unsafe {
                core::ptr::copy_nonoverlapping(
                    addr_of!(MODULE_BUFFERS)
                        .cast::<u8>()
                        .add(store_slot * MODULE_SLOT_BYTES),
                    self.xram.as_mut_ptr().add(dest),
                    MODULE_SLOT_BYTES,
                );
            }
        }
    }

    fn do_page_in(&mut self) {
        let bytes = min_usize(self.pgd_words.saturating_mul(2), ROM_SIZE);
        self.ext_bank = [0; ROM_SIZE];
        if self.pgd_src < XRAM_SIZE {
            let available = XRAM_SIZE - self.pgd_src;
            let count = min_usize(bytes, available);
            self.ext_bank[..count].copy_from_slice(&self.xram[self.pgd_src..self.pgd_src + count]);
        }
        self.ext_bank_valid = true;
    }

    fn vblank(&mut self) {
        self.copper_active_list = self.copper_pending_list;
        self.vgc_regs[(REG_STATUS - VGC_BASE) as usize] =
            self.vgc_regs[(REG_STATUS - VGC_BASE) as usize].wrapping_add(1);
        self.raise_vgc_irq(IRQ_VBLANK);
    }

    #[allow(dead_code)]
    fn trap(&mut self, opcode: u8) -> u8 {
        self.trapped = true;
        self.trap_opcode = opcode;
        self.trap_pc = self.pc;
        0
    }

    fn step(&mut self) -> u8 {
        let mut interrupt_cycles = 0;
        if self.nmi_waiting {
            self.nmi_waiting = false;
            self.irq(0xFFFA, false);
            interrupt_cycles = 6;
        }
        if interrupt_cycles == 0 && self.irq_waiting && !self.flag(I) {
            self.irq_waiting = false;
            self.irq(0xFFFE, false);
            interrupt_cycles = 6;
        }

        let opcode = self.read(self.pc);
        macro_rules! read_at {
            ($addr:expr) => {{
                let addr = $addr;
                self.read(addr)
            }};
        }
        let cycles = match opcode {
            0x00 => {
                self.pc = self.pc.wrapping_add(2);
                self.irq(0xFFFE, true);
                7
            }

            // ORA
            0x09 => {
                let v = self.imm();
                self.a |= v;
                self.set_zn(self.a);
                self.pc += 2;
                2
            }
            0x05 => {
                let v = read_at!(self.zp());
                self.a |= v;
                self.set_zn(self.a);
                self.pc += 2;
                3
            }
            0x15 => {
                let v = read_at!(self.zpx());
                self.a |= v;
                self.set_zn(self.a);
                self.pc += 2;
                4
            }
            0x0D => {
                let v = read_at!(self.abs());
                self.a |= v;
                self.set_zn(self.a);
                self.pc += 3;
                4
            }
            0x1D => {
                let (addr, c) = self.absx(true);
                let v = self.read(addr);
                self.a |= v;
                self.set_zn(self.a);
                self.pc += 3;
                4 + c
            }
            0x19 => {
                let (addr, c) = self.absy(true);
                let v = self.read(addr);
                self.a |= v;
                self.set_zn(self.a);
                self.pc += 3;
                4 + c
            }
            0x01 => {
                let v = read_at!(self.indx());
                self.a |= v;
                self.set_zn(self.a);
                self.pc += 2;
                6
            }
            0x11 => {
                let (addr, c) = self.indy(true);
                let v = self.read(addr);
                self.a |= v;
                self.set_zn(self.a);
                self.pc += 2;
                5 + c
            }
            0x12 => {
                let v = read_at!(self.zpi());
                self.a |= v;
                self.set_zn(self.a);
                self.pc += 2;
                5
            }

            // AND
            0x29 => {
                let v = self.imm();
                self.a &= v;
                self.set_zn(self.a);
                self.pc += 2;
                2
            }
            0x25 => {
                let v = read_at!(self.zp());
                self.a &= v;
                self.set_zn(self.a);
                self.pc += 2;
                3
            }
            0x35 => {
                let v = read_at!(self.zpx());
                self.a &= v;
                self.set_zn(self.a);
                self.pc += 2;
                4
            }
            0x2D => {
                let v = read_at!(self.abs());
                self.a &= v;
                self.set_zn(self.a);
                self.pc += 3;
                4
            }
            0x3D => {
                let (addr, c) = self.absx(true);
                let v = self.read(addr);
                self.a &= v;
                self.set_zn(self.a);
                self.pc += 3;
                4 + c
            }
            0x39 => {
                let (addr, c) = self.absy(true);
                let v = self.read(addr);
                self.a &= v;
                self.set_zn(self.a);
                self.pc += 3;
                4 + c
            }
            0x21 => {
                let v = read_at!(self.indx());
                self.a &= v;
                self.set_zn(self.a);
                self.pc += 2;
                6
            }
            0x31 => {
                let (addr, c) = self.indy(true);
                let v = self.read(addr);
                self.a &= v;
                self.set_zn(self.a);
                self.pc += 2;
                5 + c
            }
            0x32 => {
                let v = read_at!(self.zpi());
                self.a &= v;
                self.set_zn(self.a);
                self.pc += 2;
                5
            }

            // EOR
            0x49 => {
                let v = self.imm();
                self.a ^= v;
                self.set_zn(self.a);
                self.pc += 2;
                2
            }
            0x45 => {
                let v = read_at!(self.zp());
                self.a ^= v;
                self.set_zn(self.a);
                self.pc += 2;
                3
            }
            0x55 => {
                let v = read_at!(self.zpx());
                self.a ^= v;
                self.set_zn(self.a);
                self.pc += 2;
                4
            }
            0x4D => {
                let v = read_at!(self.abs());
                self.a ^= v;
                self.set_zn(self.a);
                self.pc += 3;
                4
            }
            0x5D => {
                let (addr, c) = self.absx(true);
                let v = self.read(addr);
                self.a ^= v;
                self.set_zn(self.a);
                self.pc += 3;
                4 + c
            }
            0x59 => {
                let (addr, c) = self.absy(true);
                let v = self.read(addr);
                self.a ^= v;
                self.set_zn(self.a);
                self.pc += 3;
                4 + c
            }
            0x41 => {
                let v = read_at!(self.indx());
                self.a ^= v;
                self.set_zn(self.a);
                self.pc += 2;
                6
            }
            0x51 => {
                let (addr, c) = self.indy(true);
                let v = self.read(addr);
                self.a ^= v;
                self.set_zn(self.a);
                self.pc += 2;
                5 + c
            }
            0x52 => {
                let v = read_at!(self.zpi());
                self.a ^= v;
                self.set_zn(self.a);
                self.pc += 2;
                5
            }

            // ADC / SBC
            0x69 => {
                let v = self.imm();
                self.adc(v);
                self.pc += 2;
                2
            }
            0x65 => {
                let v = read_at!(self.zp());
                self.adc(v);
                self.pc += 2;
                3
            }
            0x75 => {
                let v = read_at!(self.zpx());
                self.adc(v);
                self.pc += 2;
                4
            }
            0x6D => {
                let v = read_at!(self.abs());
                self.adc(v);
                self.pc += 3;
                4
            }
            0x7D => {
                let (addr, c) = self.absx(true);
                let v = self.read(addr);
                self.adc(v);
                self.pc += 3;
                4 + c
            }
            0x79 => {
                let (addr, c) = self.absy(true);
                let v = self.read(addr);
                self.adc(v);
                self.pc += 3;
                4 + c
            }
            0x61 => {
                let v = read_at!(self.indx());
                self.adc(v);
                self.pc += 2;
                6
            }
            0x71 => {
                let (addr, c) = self.indy(true);
                let v = self.read(addr);
                self.adc(v);
                self.pc += 2;
                5 + c
            }
            0x72 => {
                let v = read_at!(self.zpi());
                self.adc(v);
                self.pc += 2;
                5
            }
            0xE9 => {
                let v = self.imm();
                self.sbc(v);
                self.pc += 2;
                2
            }
            0xE5 => {
                let v = read_at!(self.zp());
                self.sbc(v);
                self.pc += 2;
                3
            }
            0xF5 => {
                let v = read_at!(self.zpx());
                self.sbc(v);
                self.pc += 2;
                4
            }
            0xED => {
                let v = read_at!(self.abs());
                self.sbc(v);
                self.pc += 3;
                4
            }
            0xFD => {
                let (addr, c) = self.absx(true);
                let v = self.read(addr);
                self.sbc(v);
                self.pc += 3;
                4 + c
            }
            0xF9 => {
                let (addr, c) = self.absy(true);
                let v = self.read(addr);
                self.sbc(v);
                self.pc += 3;
                4 + c
            }
            0xE1 => {
                let v = read_at!(self.indx());
                self.sbc(v);
                self.pc += 2;
                6
            }
            0xF1 => {
                let (addr, c) = self.indy(true);
                let v = self.read(addr);
                self.sbc(v);
                self.pc += 2;
                5 + c
            }
            0xF2 => {
                let v = read_at!(self.zpi());
                self.sbc(v);
                self.pc += 2;
                5
            }

            // Loads
            0xA9 => {
                self.a = self.imm();
                self.set_zn(self.a);
                self.pc += 2;
                2
            }
            0xA5 => {
                self.a = read_at!(self.zp());
                self.set_zn(self.a);
                self.pc += 2;
                3
            }
            0xB5 => {
                self.a = read_at!(self.zpx());
                self.set_zn(self.a);
                self.pc += 2;
                4
            }
            0xAD => {
                self.a = read_at!(self.abs());
                self.set_zn(self.a);
                self.pc += 3;
                4
            }
            0xBD => {
                let (addr, c) = self.absx(true);
                self.a = self.read(addr);
                self.set_zn(self.a);
                self.pc += 3;
                4 + c
            }
            0xB9 => {
                let (addr, c) = self.absy(true);
                self.a = self.read(addr);
                self.set_zn(self.a);
                self.pc += 3;
                4 + c
            }
            0xA1 => {
                self.a = read_at!(self.indx());
                self.set_zn(self.a);
                self.pc += 2;
                6
            }
            0xB1 => {
                let (addr, c) = self.indy(true);
                self.a = self.read(addr);
                self.set_zn(self.a);
                self.pc += 2;
                5 + c
            }
            0xB2 => {
                self.a = read_at!(self.zpi());
                self.set_zn(self.a);
                self.pc += 2;
                5
            }
            0xA2 => {
                self.x = self.imm();
                self.set_zn(self.x);
                self.pc += 2;
                2
            }
            0xA6 => {
                self.x = read_at!(self.zp());
                self.set_zn(self.x);
                self.pc += 2;
                3
            }
            0xB6 => {
                self.x = read_at!(self.zpy());
                self.set_zn(self.x);
                self.pc += 2;
                4
            }
            0xAE => {
                self.x = read_at!(self.abs());
                self.set_zn(self.x);
                self.pc += 3;
                4
            }
            0xBE => {
                let (addr, c) = self.absy(true);
                self.x = self.read(addr);
                self.set_zn(self.x);
                self.pc += 3;
                4 + c
            }
            0xA0 => {
                self.y = self.imm();
                self.set_zn(self.y);
                self.pc += 2;
                2
            }
            0xA4 => {
                self.y = read_at!(self.zp());
                self.set_zn(self.y);
                self.pc += 2;
                3
            }
            0xB4 => {
                self.y = read_at!(self.zpx());
                self.set_zn(self.y);
                self.pc += 2;
                4
            }
            0xAC => {
                self.y = read_at!(self.abs());
                self.set_zn(self.y);
                self.pc += 3;
                4
            }
            0xBC => {
                let (addr, c) = self.absx(true);
                self.y = self.read(addr);
                self.set_zn(self.y);
                self.pc += 3;
                4 + c
            }

            // Stores
            0x85 => {
                self.write(self.zp(), self.a);
                self.pc += 2;
                3
            }
            0x95 => {
                self.write(self.zpx(), self.a);
                self.pc += 2;
                4
            }
            0x8D => {
                self.write(self.abs(), self.a);
                self.pc += 3;
                4
            }
            0x9D => {
                let (addr, _) = self.absx(false);
                self.write(addr, self.a);
                self.pc += 3;
                5
            }
            0x99 => {
                let (addr, _) = self.absy(false);
                self.write(addr, self.a);
                self.pc += 3;
                5
            }
            0x81 => {
                let addr = self.indx();
                self.write(addr, self.a);
                self.pc += 2;
                6
            }
            0x91 => {
                let (addr, _) = self.indy(false);
                self.write(addr, self.a);
                self.pc += 2;
                6
            }
            0x92 => {
                let addr = self.zpi();
                self.write(addr, self.a);
                self.pc += 2;
                5
            }
            0x86 => {
                self.write(self.zp(), self.x);
                self.pc += 2;
                3
            }
            0x96 => {
                self.write(self.zpy(), self.x);
                self.pc += 2;
                4
            }
            0x8E => {
                self.write(self.abs(), self.x);
                self.pc += 3;
                4
            }
            0x84 => {
                self.write(self.zp(), self.y);
                self.pc += 2;
                3
            }
            0x94 => {
                self.write(self.zpx(), self.y);
                self.pc += 2;
                4
            }
            0x8C => {
                self.write(self.abs(), self.y);
                self.pc += 3;
                4
            }
            0x64 => {
                self.write(self.zp(), 0);
                self.pc += 2;
                3
            }
            0x74 => {
                self.write(self.zpx(), 0);
                self.pc += 2;
                4
            }
            0x9C => {
                self.write(self.abs(), 0);
                self.pc += 3;
                4
            }
            0x9E => {
                let (addr, _) = self.absx(false);
                self.write(addr, 0);
                self.pc += 3;
                5
            }

            // Compare
            0xC9 => {
                self.compare(self.a, self.imm());
                self.pc += 2;
                2
            }
            0xC5 => {
                let v = read_at!(self.zp());
                self.compare(self.a, v);
                self.pc += 2;
                3
            }
            0xD5 => {
                let v = read_at!(self.zpx());
                self.compare(self.a, v);
                self.pc += 2;
                4
            }
            0xCD => {
                let v = read_at!(self.abs());
                self.compare(self.a, v);
                self.pc += 3;
                4
            }
            0xDD => {
                let (addr, c) = self.absx(true);
                let v = self.read(addr);
                self.compare(self.a, v);
                self.pc += 3;
                4 + c
            }
            0xD9 => {
                let (addr, c) = self.absy(true);
                let v = self.read(addr);
                self.compare(self.a, v);
                self.pc += 3;
                4 + c
            }
            0xC1 => {
                let v = read_at!(self.indx());
                self.compare(self.a, v);
                self.pc += 2;
                6
            }
            0xD1 => {
                let (addr, c) = self.indy(true);
                let v = self.read(addr);
                self.compare(self.a, v);
                self.pc += 2;
                5 + c
            }
            0xD2 => {
                let v = read_at!(self.zpi());
                self.compare(self.a, v);
                self.pc += 2;
                5
            }
            0xE0 => {
                self.compare(self.x, self.imm());
                self.pc += 2;
                2
            }
            0xE4 => {
                let v = read_at!(self.zp());
                self.compare(self.x, v);
                self.pc += 2;
                3
            }
            0xEC => {
                let v = read_at!(self.abs());
                self.compare(self.x, v);
                self.pc += 3;
                4
            }
            0xC0 => {
                self.compare(self.y, self.imm());
                self.pc += 2;
                2
            }
            0xC4 => {
                let v = read_at!(self.zp());
                self.compare(self.y, v);
                self.pc += 2;
                3
            }
            0xCC => {
                let v = read_at!(self.abs());
                self.compare(self.y, v);
                self.pc += 3;
                4
            }

            // INC / DEC
            0xE6 => {
                let addr = self.zp();
                let old = self.read(addr);
                let v = self.inc(old);
                self.write(addr, v);
                self.pc += 2;
                5
            }
            0xF6 => {
                let addr = self.zpx();
                let old = self.read(addr);
                let v = self.inc(old);
                self.write(addr, v);
                self.pc += 2;
                6
            }
            0xEE => {
                let addr = self.abs();
                let old = self.read(addr);
                let v = self.inc(old);
                self.write(addr, v);
                self.pc += 3;
                6
            }
            0xFE => {
                let (addr, _) = self.absx(false);
                let old = self.read(addr);
                let v = self.inc(old);
                self.write(addr, v);
                self.pc += 3;
                7
            }
            0x1A => {
                self.a = self.inc(self.a);
                self.pc += 1;
                2
            }
            0xC6 => {
                let addr = self.zp();
                let old = self.read(addr);
                let v = self.dec(old);
                self.write(addr, v);
                self.pc += 2;
                5
            }
            0xD6 => {
                let addr = self.zpx();
                let old = self.read(addr);
                let v = self.dec(old);
                self.write(addr, v);
                self.pc += 2;
                6
            }
            0xCE => {
                let addr = self.abs();
                let old = self.read(addr);
                let v = self.dec(old);
                self.write(addr, v);
                self.pc += 3;
                6
            }
            0xDE => {
                let (addr, _) = self.absx(false);
                let old = self.read(addr);
                let v = self.dec(old);
                self.write(addr, v);
                self.pc += 3;
                7
            }
            0x3A => {
                self.a = self.dec(self.a);
                self.pc += 1;
                2
            }

            // Shifts / rotates
            0x0A => {
                self.a = self.asl(self.a);
                self.pc += 1;
                2
            }
            0x06 => {
                let addr = self.zp();
                let old = self.read(addr);
                let v = self.asl(old);
                self.write(addr, v);
                self.pc += 2;
                5
            }
            0x16 => {
                let addr = self.zpx();
                let old = self.read(addr);
                let v = self.asl(old);
                self.write(addr, v);
                self.pc += 2;
                6
            }
            0x0E => {
                let addr = self.abs();
                let old = self.read(addr);
                let v = self.asl(old);
                self.write(addr, v);
                self.pc += 3;
                6
            }
            0x1E => {
                let (addr, _) = self.absx(false);
                let old = self.read(addr);
                let v = self.asl(old);
                self.write(addr, v);
                self.pc += 3;
                7
            }
            0x4A => {
                self.a = self.lsr(self.a);
                self.pc += 1;
                2
            }
            0x46 => {
                let addr = self.zp();
                let old = self.read(addr);
                let v = self.lsr(old);
                self.write(addr, v);
                self.pc += 2;
                5
            }
            0x56 => {
                let addr = self.zpx();
                let old = self.read(addr);
                let v = self.lsr(old);
                self.write(addr, v);
                self.pc += 2;
                6
            }
            0x4E => {
                let addr = self.abs();
                let old = self.read(addr);
                let v = self.lsr(old);
                self.write(addr, v);
                self.pc += 3;
                6
            }
            0x5E => {
                let (addr, _) = self.absx(false);
                let old = self.read(addr);
                let v = self.lsr(old);
                self.write(addr, v);
                self.pc += 3;
                7
            }
            0x2A => {
                self.a = self.rol(self.a);
                self.pc += 1;
                2
            }
            0x26 => {
                let addr = self.zp();
                let old = self.read(addr);
                let v = self.rol(old);
                self.write(addr, v);
                self.pc += 2;
                5
            }
            0x36 => {
                let addr = self.zpx();
                let old = self.read(addr);
                let v = self.rol(old);
                self.write(addr, v);
                self.pc += 2;
                6
            }
            0x2E => {
                let addr = self.abs();
                let old = self.read(addr);
                let v = self.rol(old);
                self.write(addr, v);
                self.pc += 3;
                6
            }
            0x3E => {
                let (addr, _) = self.absx(false);
                let old = self.read(addr);
                let v = self.rol(old);
                self.write(addr, v);
                self.pc += 3;
                7
            }
            0x6A => {
                self.a = self.ror(self.a);
                self.pc += 1;
                2
            }
            0x66 => {
                let addr = self.zp();
                let old = self.read(addr);
                let v = self.ror(old);
                self.write(addr, v);
                self.pc += 2;
                5
            }
            0x76 => {
                let addr = self.zpx();
                let old = self.read(addr);
                let v = self.ror(old);
                self.write(addr, v);
                self.pc += 2;
                6
            }
            0x6E => {
                let addr = self.abs();
                let old = self.read(addr);
                let v = self.ror(old);
                self.write(addr, v);
                self.pc += 3;
                6
            }
            0x7E => {
                let (addr, _) = self.absx(false);
                let old = self.read(addr);
                let v = self.ror(old);
                self.write(addr, v);
                self.pc += 3;
                7
            }

            // Branches
            0x10 => self.branch(!self.flag(N)),
            0x30 => self.branch(self.flag(N)),
            0x50 => self.branch(!self.flag(V)),
            0x70 => self.branch(self.flag(V)),
            0x90 => self.branch(!self.flag(C)),
            0xB0 => self.branch(self.flag(C)),
            0xD0 => self.branch(!self.flag(Z)),
            0xF0 => self.branch(self.flag(Z)),
            0x80 => self.branch(true),

            // BIT / TSB / TRB
            0x24 => {
                let v = read_at!(self.zp());
                self.bit(v, true);
                self.pc += 2;
                3
            }
            0x2C => {
                let v = read_at!(self.abs());
                self.bit(v, true);
                self.pc += 3;
                4
            }
            0x89 => {
                self.bit(self.imm(), false);
                self.pc += 2;
                2
            }
            0x34 => {
                let v = read_at!(self.zpx());
                self.bit(v, true);
                self.pc += 2;
                4
            }
            0x3C => {
                let (addr, _) = self.absx(false);
                let v = self.read(addr);
                self.bit(v, true);
                self.pc += 3;
                4
            }
            0x04 => {
                let addr = self.zp();
                let v = self.read(addr);
                self.set_flag(Z, self.a & v == 0);
                self.write(addr, self.a | v);
                self.pc += 2;
                5
            }
            0x0C => {
                let addr = self.abs();
                let v = self.read(addr);
                self.set_flag(Z, self.a & v == 0);
                self.write(addr, self.a | v);
                self.pc += 3;
                6
            }
            0x14 => {
                let addr = self.zp();
                let v = self.read(addr);
                self.set_flag(Z, self.a & v == 0);
                self.write(addr, !self.a & v);
                self.pc += 2;
                5
            }
            0x1C => {
                let addr = self.abs();
                let v = self.read(addr);
                self.set_flag(Z, self.a & v == 0);
                self.write(addr, !self.a & v);
                self.pc += 3;
                6
            }

            // Rockwell/WDC 65C02 bit operations
            0x07 | 0x17 | 0x27 | 0x37 | 0x47 | 0x57 | 0x67 | 0x77 => {
                let addr = self.zp();
                let mask = bit_mask(opcode);
                let value = self.read(addr) & !mask;
                self.write(addr, value);
                self.pc += 2;
                5
            }
            0x87 | 0x97 | 0xA7 | 0xB7 | 0xC7 | 0xD7 | 0xE7 | 0xF7 => {
                let addr = self.zp();
                let mask = bit_mask(opcode);
                let value = self.read(addr) | mask;
                self.write(addr, value);
                self.pc += 2;
                5
            }
            0x0F | 0x1F | 0x2F | 0x3F | 0x4F | 0x5F | 0x6F | 0x7F => {
                self.bit_branch(bit_mask(opcode), false)
            }
            0x8F | 0x9F | 0xAF | 0xBF | 0xCF | 0xDF | 0xEF | 0xFF => {
                self.bit_branch(bit_mask(opcode), true)
            }

            // Jumps / calls / returns
            0x4C => {
                self.pc = self.abs();
                3
            }
            0x6C => {
                self.pc = self.read_word(self.abs());
                5
            }
            0x7C => {
                let (addr, _) = self.absx(false);
                self.pc = self.read_word(addr);
                6
            }
            0x20 => {
                self.push_word(self.pc.wrapping_add(2));
                self.pc = self.abs();
                6
            }
            0x60 => {
                self.pc = self.pop_word().wrapping_add(1);
                6
            }
            0x40 => {
                self.p = (self.pop() & !B) | U;
                self.pc = self.pop_word();
                6
            }

            // Flags / transfers / register inc-dec
            0x18 => {
                self.set_flag(C, false);
                self.pc += 1;
                2
            }
            0x38 => {
                self.set_flag(C, true);
                self.pc += 1;
                2
            }
            0x58 => {
                self.set_flag(I, false);
                self.pc += 1;
                2
            }
            0x78 => {
                self.set_flag(I, true);
                self.pc += 1;
                2
            }
            0xB8 => {
                self.set_flag(V, false);
                self.pc += 1;
                2
            }
            0xD8 => {
                self.set_flag(D, false);
                self.pc += 1;
                2
            }
            0xF8 => {
                self.set_flag(D, true);
                self.pc += 1;
                2
            }
            0xAA => {
                self.x = self.a;
                self.set_zn(self.x);
                self.pc += 1;
                2
            }
            0xA8 => {
                self.y = self.a;
                self.set_zn(self.y);
                self.pc += 1;
                2
            }
            0x8A => {
                self.a = self.x;
                self.set_zn(self.a);
                self.pc += 1;
                2
            }
            0x98 => {
                self.a = self.y;
                self.set_zn(self.a);
                self.pc += 1;
                2
            }
            0xBA => {
                self.x = self.sp;
                self.set_zn(self.x);
                self.pc += 1;
                2
            }
            0x9A => {
                self.sp = self.x;
                self.pc += 1;
                2
            }
            0xE8 => {
                self.x = self.x.wrapping_add(1);
                self.set_zn(self.x);
                self.pc += 1;
                2
            }
            0xC8 => {
                self.y = self.y.wrapping_add(1);
                self.set_zn(self.y);
                self.pc += 1;
                2
            }
            0xCA => {
                self.x = self.x.wrapping_sub(1);
                self.set_zn(self.x);
                self.pc += 1;
                2
            }
            0x88 => {
                self.y = self.y.wrapping_sub(1);
                self.set_zn(self.y);
                self.pc += 1;
                2
            }

            // Stack
            0x48 => {
                self.push(self.a);
                self.pc += 1;
                3
            }
            0x08 => {
                self.push(self.p | B | U);
                self.pc += 1;
                3
            }
            0xDA => {
                self.push(self.x);
                self.pc += 1;
                3
            }
            0x5A => {
                self.push(self.y);
                self.pc += 1;
                3
            }
            0x68 => {
                self.a = self.pop();
                self.set_zn(self.a);
                self.pc += 1;
                4
            }
            0x28 => {
                self.p = (self.pop() & !B) | U;
                self.pc += 1;
                4
            }
            0xFA => {
                self.x = self.pop();
                self.set_zn(self.x);
                self.pc += 1;
                4
            }
            0x7A => {
                self.y = self.pop();
                self.set_zn(self.y);
                self.pc += 1;
                4
            }

            // NOP and reserved 65C02-ish NOP byte lengths.
            0xEA => {
                self.pc += 1;
                2
            }
            0x02 | 0x22 | 0x42 | 0x62 | 0x82 | 0xC2 | 0xE2 => {
                self.pc += 2;
                2
            }
            0x03 | 0x13 | 0x23 | 0x33 | 0x43 | 0x53 | 0x63 | 0x73 | 0x83 | 0x93 | 0xA3 | 0xB3
            | 0xC3 | 0xD3 | 0xE3 | 0xF3 | 0x0B | 0x1B | 0x2B | 0x3B | 0x4B | 0x5B | 0x6B | 0x7B
            | 0x8B | 0x9B | 0xAB | 0xBB | 0xEB | 0xFB => {
                self.pc += 1;
                1
            }
            0x44 => {
                self.pc += 2;
                3
            }
            0x54 | 0xD4 | 0xF4 => {
                self.pc += 2;
                4
            }
            0x5C => {
                self.pc += 3;
                8
            }
            0xDC | 0xFC => {
                self.pc += 3;
                4
            }
            0xCB | 0xDB => {
                self.pc += 1;
                3
            }
        };
        cycles + interrupt_cycles
    }

    fn read(&mut self, addr: u16) -> u8 {
        if self.flat_bus {
            return self.ram[addr as usize];
        }
        if addr < VGC_BASE || addr >= ROM_BASE {
            return self.ram[addr as usize];
        }
        self.read_io(addr)
    }

    fn write(&mut self, addr: u16, value: u8) {
        if self.flat_bus {
            self.ram[addr as usize] = value;
            return;
        }
        if addr < VGC_BASE {
            self.ram[addr as usize] = value;
            return;
        }
        if addr >= ROM_BASE {
            return;
        }
        self.write_io(addr, value);
    }

    fn read_io(&mut self, addr: u16) -> u8 {
        if (REG_MODE..=REG_CHAR_IN).contains(&addr) {
            match addr {
                REG_CHAR_IN => return self.dequeue_byte().unwrap_or(0),
                REG_COL_ST => return self.collision_sprite as u8,
                REG_COL_BG => return self.collision_background as u8,
                _ => return self.vgc_regs[(addr - VGC_BASE) as usize],
            }
        }
        if (REG_HELP..=HELP_SEARCH_END).contains(&addr) || (CMP_BASE..=CMP_END).contains(&addr) {
            return self.ram[addr as usize];
        }
        if (SPRITE_REG_BASE..=SPRITE_REG_END).contains(&addr) {
            return self.read_sprite_register(addr);
        }
        if (VRAM_PLANE..=VRAM_REG_END).contains(&addr) {
            return self.read_vram_register(addr);
        }
        match addr {
            DISPLAY_DIM => return self.display_dim,
            REG_TEXT_FLAGS => return self.text_flags,
            REG_TEXT_REVERSE_ATTR => return self.text_reverse_attr,
            REG_GFX_TRANSPARENT_COLOR => return self.gfx_transparent_color,
            REG_PALETTE_MODE => return self.palette_mode,
            REG_SCROLL_CTL => return self.scroll_ctl & 0x07,
            REG_COL_ST_HI => return (self.collision_sprite >> 8) as u8,
            REG_COL_BG_HI => return (self.collision_background >> 8) as u8,
            REG_TEXT_TOP_ROW => return self.text_top_row,
            REG_TEXT_SCROLL_START => return self.text_scroll_start,
            REG_TEXT_SCROLL_ROWS => return self.text_scroll_rows,
            _ => {}
        }
        if (VGC_IRQ_BASE..=VGC_IRQ_END).contains(&addr) {
            return self.read_vgc_irq_register(addr);
        }
        if (NIC_BASE..=NIC_END).contains(&addr) {
            return self.read_nic_register(addr);
        }
        if (WTS_BASE..=WTS_END).contains(&addr) {
            return self.read_wts_register(addr);
        }
        if (REG_CMD..=REG_P14).contains(&addr) {
            if addr == REG_CMD {
                return 0;
            }
            return self.cmd_regs[(addr - REG_CMD) as usize];
        }
        if (SCREEN_WIN_BASE..=SCREEN_WIN_END).contains(&addr) {
            let offset = (addr - SCREEN_WIN_BASE) as usize;
            return self.read_screen_window(offset);
        }
        if addr == SCREEN_WIN_PLANE_SEL {
            return self.screen_win_plane;
        }
        if (FIO_BASE..=FIO_END).contains(&addr) {
            return self.ram[addr as usize];
        }
        if (XMC_BASE..=XMC_END).contains(&addr) {
            return self.read_xmc_register(addr);
        }
        if (TIMER_BASE..=TIMER_END).contains(&addr) {
            return self.read_timer_register(addr);
        }
        if (MUSIC_STATUS..=MUSIC_TOTAL_H).contains(&addr) {
            return 0;
        }
        if (DMA_BASE..=DMA_END).contains(&addr) {
            return self.dma_regs[(addr - DMA_BASE) as usize];
        }
        if (XMC_WINDOW_BASE..=XMC_WINDOW_END).contains(&addr) {
            return self.read_xmc_window(addr);
        }
        match addr {
            PGD_STATUS => return PGD_STATUS_DONE,
            PGD_SRC_L => return (self.pgd_src & 0xFF) as u8,
            PGD_SRC_M => return ((self.pgd_src >> 8) & 0xFF) as u8,
            PGD_SRC_H => return ((self.pgd_src >> 16) & 0xFF) as u8,
            PGD_WORDS_L => return (self.pgd_words & 0xFF) as u8,
            PGD_WORDS_H => return ((self.pgd_words >> 8) & 0xFF) as u8,
            _ => {}
        }
        if (BLT_BASE..=BLT_END).contains(&addr) || addr == BLT_ROTATE_ANGLE {
            return self.read_blt_register(addr);
        }
        if (BOARD_INPUT_BASE..=BOARD_INPUT_END).contains(&addr) {
            return self.read_board_register(addr);
        }
        if (USB_HID_DIAG_BASE..=USB_HID_DIAG_END).contains(&addr) {
            return 0;
        }
        if (META_BASE..=META_END).contains(&addr)
            || (MATH_BASE..=MATH_END).contains(&addr)
            || (MUSIC_META_SOUNDFONT..=MUSIC_META_SOUNDFONT_END).contains(&addr)
        {
            if (MATH_BASE..=MATH_END).contains(&addr) {
                return self.read_math_register(addr);
            }
            return self.ram[addr as usize];
        }
        self.ram[addr as usize]
    }

    fn write_io(&mut self, addr: u16, value: u8) {
        if addr == REG_ROMSWAP {
            match value {
                ROMSWAP_EXTENSION => self.map_extension_rom(),
                ROMSWAP_BASIC | ROMSWAP_LOGO | ROMSWAP_FORTH => self.map_basic_rom(),
                _ => {}
            }
            return;
        }
        if (REG_HELP..=HELP_SEARCH_END).contains(&addr) || (CMP_BASE..=CMP_END).contains(&addr) {
            if addr != REG_HELP {
                self.ram[addr as usize] = value;
            }
            return;
        }
        if (SPRITE_REG_BASE..=SPRITE_REG_END).contains(&addr) {
            self.write_sprite_register(addr, value);
            return;
        }
        if (REG_CMD..=REG_P14).contains(&addr) {
            if addr == REG_CMD {
                self.cmd_regs[0] = value;
                self.execute_vgc_command(value);
            } else {
                self.cmd_regs[(addr - REG_CMD) as usize] = value;
            }
            return;
        }
        if (VRAM_PLANE..=VRAM_REG_END).contains(&addr) {
            self.write_vram_register(addr, value);
            return;
        }
        match addr {
            DISPLAY_DIM => {
                let next = value & 0x0F;
                if self.display_dim != next {
                    self.display_dim = next;
                    self.dirty();
                }
                return;
            }
            REG_TEXT_FLAGS => {
                if self.text_flags != value {
                    self.text_flags = value;
                    self.dirty();
                }
                return;
            }
            REG_TEXT_REVERSE_ATTR => {
                if self.text_reverse_attr != value {
                    self.text_reverse_attr = value;
                    self.dirty();
                }
                return;
            }
            REG_GFX_TRANSPARENT_COLOR => {
                let next = value & 0x0F;
                if self.gfx_transparent_color != next {
                    self.gfx_transparent_color = next;
                    self.dirty();
                }
                return;
            }
            REG_PALETTE_MODE => {
                let next = value & 0x01;
                if self.palette_mode != next {
                    self.palette_mode = next;
                    self.dirty();
                }
                return;
            }
            REG_SCROLL_CTL => {
                let next = value & 0x07;
                if self.scroll_ctl != next {
                    self.scroll_ctl = next;
                    self.dirty();
                }
                return;
            }
            REG_COL_ST_HI => {
                self.collision_sprite &= 0x00FF;
                self.update_irq_line();
                return;
            }
            REG_COL_BG_HI => {
                self.collision_background &= 0x00FF;
                self.update_irq_line();
                return;
            }
            REG_TEXT_TOP_ROW => {
                let rows = self.text_scroll_rows.max(1);
                self.text_top_row = value % rows;
                self.dirty();
                return;
            }
            REG_TEXT_SCROLL_START => {
                self.text_scroll_start = min_u8(value, (SCREEN_ROWS - 1) as u8);
                let max_rows = (SCREEN_ROWS as u8)
                    .saturating_sub(self.text_scroll_start)
                    .max(1);
                self.text_scroll_rows = min_u8(self.text_scroll_rows.max(1), max_rows);
                self.text_top_row %= self.text_scroll_rows.max(1);
                self.dirty();
                return;
            }
            REG_TEXT_SCROLL_ROWS => {
                let max_rows = (SCREEN_ROWS as u8)
                    .saturating_sub(self.text_scroll_start)
                    .max(1);
                self.text_scroll_rows = if value == 0 {
                    max_rows
                } else {
                    min_u8(value, max_rows).max(1)
                };
                self.text_top_row %= self.text_scroll_rows.max(1);
                self.dirty();
                return;
            }
            _ => {}
        }
        if (VGC_IRQ_BASE..=VGC_IRQ_END).contains(&addr) {
            self.write_vgc_irq_register(addr, value);
            return;
        }
        if (NIC_BASE..=NIC_END).contains(&addr) {
            self.write_nic_register(addr, value);
            return;
        }
        if (WTS_BASE..=WTS_END).contains(&addr) {
            self.write_wts_register(addr, value);
            return;
        }
        if (SCREEN_WIN_BASE..=SCREEN_WIN_END).contains(&addr) {
            let offset = (addr - SCREEN_WIN_BASE) as usize;
            self.write_screen_window(offset, value);
            return;
        }
        if addr == SCREEN_WIN_PLANE_SEL {
            self.screen_win_plane = value;
            return;
        }
        if (FIO_BASE..=FIO_END).contains(&addr) {
            self.write_fio(addr, value);
            return;
        }
        if (XMC_BASE..=XMC_END).contains(&addr) {
            self.write_xmc_register(addr, value);
            return;
        }
        if (TIMER_BASE..=TIMER_END).contains(&addr) {
            self.write_timer_register(addr, value);
            return;
        }
        if (MUSIC_STATUS..=MUSIC_TOTAL_H).contains(&addr) {
            return;
        }
        if (DMA_BASE..=DMA_END).contains(&addr) {
            self.write_dma_register(addr, value);
            return;
        }
        if (XMC_WINDOW_BASE..=XMC_WINDOW_END).contains(&addr) {
            self.write_xmc_window(addr, value);
            return;
        }
        match addr {
            PGD_SRC_L => {
                self.pgd_src = (self.pgd_src & !0x0000FF) | value as usize;
                return;
            }
            PGD_SRC_M => {
                self.pgd_src = (self.pgd_src & !0x00FF00) | ((value as usize) << 8);
                return;
            }
            PGD_SRC_H => {
                self.pgd_src = (self.pgd_src & !0xFF0000) | ((value as usize) << 16);
                return;
            }
            PGD_WORDS_L => {
                self.pgd_words = (self.pgd_words & !0x00FF) | value as usize;
                return;
            }
            PGD_WORDS_H => {
                self.pgd_words = (self.pgd_words & !0xFF00) | ((value as usize) << 8);
                return;
            }
            PGD_CMD => {
                if value & PGD_START != 0 {
                    self.do_page_in();
                }
                return;
            }
            _ => {}
        }
        if (BLT_BASE..=BLT_END).contains(&addr) || addr == BLT_ROTATE_ANGLE {
            self.write_blt_register(addr, value);
            return;
        }
        if (BOARD_INPUT_BASE..=BOARD_INPUT_END).contains(&addr) {
            self.write_board_register(addr, value);
            return;
        }
        if (USB_HID_DIAG_BASE..=USB_HID_DIAG_END).contains(&addr) {
            return;
        }
        if (META_BASE..=META_END).contains(&addr)
            || (MATH_BASE..=MATH_END).contains(&addr)
            || (MUSIC_META_SOUNDFONT..=MUSIC_META_SOUNDFONT_END).contains(&addr)
        {
            if (MATH_BASE..=MATH_END).contains(&addr) {
                self.write_math_register(addr, value);
            } else {
                self.ram[addr as usize] = value;
            }
            return;
        }
        if !(REG_MODE..=REG_CHAR_IN).contains(&addr) {
            self.ram[addr as usize] = value;
            return;
        }

        let index = (addr - VGC_BASE) as usize;
        match addr {
            REG_STATUS => {}
            REG_SPRITE_COUNT => {}
            REG_COL_ST => {
                self.collision_sprite &= 0xFF00;
                self.update_irq_line();
            }
            REG_COL_BG => {
                self.collision_background &= 0xFF00;
                self.update_irq_line();
            }
            REG_CHAR_OUT => {
                self.vgc_regs[index] = value;
                self.char_out(value);
            }
            REG_CURSOR_X => {
                let next = min_u8(value, (SCREEN_COLS - 1) as u8);
                if self.vgc_regs[index] != next {
                    self.vgc_regs[index] = next;
                    self.dirty();
                }
            }
            REG_CURSOR_Y => {
                let next = min_u8(value & 0x3F, (SCREEN_ROWS - 1) as u8);
                if self.vgc_regs[index] != next {
                    self.vgc_regs[index] = next;
                    self.dirty();
                }
            }
            REG_CURSOR_ENABLE => {
                let next = value & 1;
                if self.vgc_regs[index] != next {
                    self.vgc_regs[index] = next;
                    self.dirty();
                }
            }
            _ => {
                if self.vgc_regs[index] != value {
                    self.vgc_regs[index] = value;
                    self.dirty();
                }
            }
        }
    }

    fn execute_vgc_command(&mut self, command: u8) {
        match command {
            CMD_SPR_DEF..=CMD_SPR_PRI => self.execute_sprite_command(command),
            CMD_PLOT => self.plot(
                self.param_u16(1) as i32,
                self.param_u16(3) as i32,
                self.gfx_draw_color,
            ),
            CMD_UNPLOT => self.plot(self.param_u16(1) as i32, self.param_u16(3) as i32, 0),
            CMD_LINE => {
                self.line(
                    (self.param_u16(1) as i16) as i32,
                    (self.param_u16(3) as i16) as i32,
                    (self.param_u16(5) as i16) as i32,
                    (self.param_u16(7) as i16) as i32,
                    self.gfx_draw_color,
                );
            }
            CMD_CIRCLE => {
                let rx = self.param_u16(5) as i32;
                let mut ry = self.param_u16(7) as i32;
                if ry == 0 {
                    ry = rx;
                }
                self.ellipse(
                    self.param_u16(1) as i32,
                    self.param_u16(3) as i32,
                    rx,
                    ry,
                    self.gfx_draw_color,
                );
            }
            CMD_RECT => self.rect(
                self.param_u16(1) as i32,
                self.param_u16(3) as i32,
                self.param_u16(5) as i32,
                self.param_u16(7) as i32,
                self.gfx_draw_color,
            ),
            CMD_FILL => self.fill_rect(
                self.param_u16(1) as i32,
                self.param_u16(3) as i32,
                self.param_u16(5) as i32,
                self.param_u16(7) as i32,
                self.gfx_draw_color,
            ),
            CMD_GCLS => self.clear_gfx(),
            CMD_GCOLOR => {
                self.gfx_draw_color = self.cmd_regs[1] & 0x0F;
            }
            CMD_PAINT => self.flood_fill(
                self.param_u16(1) as i32,
                self.param_u16(3) as i32,
                self.gfx_draw_color,
            ),
            CMD_MEM_READ | CMD_MEM_WRITE => self.execute_memory_command(command),
            CMD_COPPER_ADD | CMD_COPPER_CLEAR | CMD_COPPER_ENABLE | CMD_COPPER_DISABLE
            | CMD_COPPER_LIST | CMD_COPPER_USE | CMD_COPPER_LIST_END => {
                self.execute_copper_command(command);
            }
            CMD_SYS_RESET => self.reset_io_devices(),
            _ => {}
        }
        self.cmd_regs[0] = 0;
    }

    fn reset_io_devices(&mut self) {
        self.screen.fill(0x20);
        self.color.fill(0x0F);
        self.attr.fill(0);
        self.gfx.fill(0);
        self.sprite_shapes.fill(0);
        self.sprite_regs.fill(0);
        for i in 0..MAX_SPRITES {
            let base = i * SPRITE_REG_STRIDE;
            self.sprite_regs[base + SPR_REG_SHAPE] = i as u8;
            self.sprite_regs[base + SPR_REG_PRIORITY] = 2;
        }
        self.vgc_regs.fill(0);
        self.vgc_regs[(REG_FG - VGC_BASE) as usize] = 15;
        self.vgc_regs[(REG_BG - VGC_BASE) as usize] = 0;
        self.vgc_regs[(REG_BORDER - VGC_BASE) as usize] = 11;
        self.gfx_draw_color = 1;
        self.gfx_transparent_color = 0;
        self.palette_mode = 0;
        self.display_dim = 15;
        self.text_flags = 0;
        self.text_reverse_attr = 0xF0;
        self.scroll_ctl = 0x06;
        self.text_top_row = 0;
        self.text_scroll_start = 0;
        self.text_scroll_rows = SCREEN_ROWS as u8;
        self.copper_counts = [0; COPPER_LIST_COUNT];
        self.copper_target_list = 0;
        self.copper_active_list = 0;
        self.copper_pending_list = 0;
        self.copper_enabled = false;
        self.irq_enable = 0;
        self.irq_status = 0;
        self.timer_regs.fill(0);
        self.dma_regs.fill(0);
        self.blt_regs.fill(0);
        self.board_regs.fill(0);
        self.dma_regs[(DMA_STATUS - DMA_BASE) as usize] = DMA_STATUS_IDLE;
        self.blt_regs[Self::blt_index(BLT_STATUS)] = BLT_STATUS_IDLE;
        self.timer_counter = 0;
        self.timer_cycle_accumulator = 0;
        self.update_irq_line();
        self.dirty();
    }

    fn param_u16(&self, first: usize) -> u16 {
        self.cmd_regs[first] as u16 | ((self.cmd_regs[first + 1] as u16) << 8)
    }

    fn execute_memory_command(&mut self, command: u8) {
        let space = self.cmd_regs[1];
        let mut addr = self.cmd_regs[2] as u16 | ((self.cmd_regs[3] as u16) << 8);
        if command == CMD_MEM_READ {
            self.cmd_regs[4] = self.read_memory_space(space, addr).unwrap_or(0);
        } else {
            self.write_memory_space(space, addr, self.cmd_regs[4]);
        }

        if self.cmd_regs[5] & 0x01 != 0 {
            addr = addr.wrapping_add(1);
            self.cmd_regs[2] = addr as u8;
            self.cmd_regs[3] = (addr >> 8) as u8;
        }
    }

    fn execute_copper_command(&mut self, command: u8) {
        match command {
            CMD_COPPER_ADD => self.add_copper_event_from_registers(),
            CMD_COPPER_CLEAR => {
                self.copper_counts[self.copper_target_list] = 0;
                self.dirty();
            }
            CMD_COPPER_ENABLE => {
                if !self.copper_enabled {
                    self.copper_enabled = true;
                    self.dirty();
                }
            }
            CMD_COPPER_DISABLE => {
                if self.copper_enabled {
                    self.copper_enabled = false;
                    self.dirty();
                }
            }
            CMD_COPPER_LIST => {
                self.copper_target_list = (self.cmd_regs[1] & 0x7F) as usize;
            }
            CMD_COPPER_USE => {
                self.copper_pending_list = (self.cmd_regs[1] & 0x7F) as usize;
            }
            CMD_COPPER_LIST_END => {
                self.copper_target_list = self.copper_active_list;
            }
            _ => {}
        }
    }

    fn add_copper_event_from_registers(&mut self) {
        let x = self.param_u16(1) as usize;
        let y = self.cmd_regs[3] as usize;
        let register_specifier = self.param_u16(4);
        let value = self.cmd_regs[6];

        if x >= GFX_WIDTH || y >= GFX_HEIGHT {
            return;
        }
        let Some(register_index) = resolve_copper_register(register_specifier) else {
            return;
        };

        let position = (y * GFX_WIDTH + x) as u16;
        let list = self.copper_target_list;
        let count = self.copper_counts[list] as usize;

        for i in 0..count {
            let event = self.copper_events[list][i];
            if event.position == position && event.register_index == register_index {
                self.copper_events[list][i].value = value;
                self.dirty();
                return;
            }
        }

        if count >= MAX_COPPER_EVENTS_PER_LIST {
            return;
        }

        let mut insert_at = count;
        for i in 0..count {
            let event = self.copper_events[list][i];
            if position < event.position
                || (position == event.position && register_index < event.register_index)
            {
                insert_at = i;
                break;
            }
        }

        let mut i = count;
        while i > insert_at {
            self.copper_events[list][i] = self.copper_events[list][i - 1];
            i -= 1;
        }
        self.copper_events[list][insert_at] = CopperEvent {
            position,
            register_index,
            value,
        };
        self.copper_counts[list] = (count + 1) as u16;
        self.dirty();
    }

    fn read_vram_register(&mut self, addr: u16) -> u8 {
        match addr {
            VRAM_PLANE => self.vram_plane,
            VRAM_ADDR_L => self.vram_addr as u8,
            VRAM_ADDR_H => (self.vram_addr >> 8) as u8,
            VRAM_DATA => self.read_vram_data(),
            VRAM_CTRL => self.vram_ctrl,
            _ => 0,
        }
    }

    fn read_vram_data(&mut self) -> u8 {
        let result = self.vram_read_latch;
        self.vram_read_latch = self
            .read_memory_space(self.vram_plane, self.vram_addr)
            .unwrap_or(0);
        if self.vram_ctrl & VRAM_CTRL_AUTO_INC != 0 {
            self.vram_addr = self.vram_addr.wrapping_add(1);
        }
        result
    }

    fn write_vram_register(&mut self, addr: u16, value: u8) {
        match addr {
            VRAM_PLANE => self.vram_plane = value,
            VRAM_ADDR_L => self.vram_addr = (self.vram_addr & 0xFF00) | value as u16,
            VRAM_ADDR_H => self.vram_addr = (self.vram_addr & 0x00FF) | ((value as u16) << 8),
            VRAM_CTRL => self.vram_ctrl = value,
            VRAM_DATA => {
                self.write_memory_space(self.vram_plane, self.vram_addr, value);
                if self.vram_ctrl & VRAM_CTRL_AUTO_INC != 0 {
                    self.vram_addr = self.vram_addr.wrapping_add(1);
                }
            }
            _ => {}
        }
    }

    fn read_memory_space(&self, space: u8, addr: u16) -> Option<u8> {
        let index = addr as usize;
        match space {
            MEM_SPACE_SCREEN if index < SCREEN_SIZE => Some(self.screen[index]),
            MEM_SPACE_COLOR if index < SCREEN_SIZE => Some(self.color[index]),
            MEM_SPACE_TEXT_ATTR if index < SCREEN_SIZE => Some(self.attr[index]),
            MEM_SPACE_GFX if index < GFX_SIZE => Some(self.gfx[index]),
            MEM_SPACE_SPRITE if index < SPRITE_SHAPE_RAM_SIZE => Some(self.sprite_shapes[index]),
            _ => None,
        }
    }

    fn write_memory_space(&mut self, space: u8, addr: u16, value: u8) -> bool {
        let index = addr as usize;
        match space {
            MEM_SPACE_SCREEN if index < SCREEN_SIZE => self.set_screen_cell(index, value),
            MEM_SPACE_COLOR if index < SCREEN_SIZE => self.set_color_cell(index, value),
            MEM_SPACE_TEXT_ATTR if index < SCREEN_SIZE => self.set_attr_cell(index, value),
            MEM_SPACE_GFX if index < GFX_SIZE => self.set_gfx_index(index, value & 0x0F),
            MEM_SPACE_SPRITE if index < SPRITE_SHAPE_RAM_SIZE => {
                if self.sprite_shapes[index] != value {
                    self.sprite_shapes[index] = value;
                    self.dirty();
                }
                true
            }
            _ => false,
        }
    }

    fn set_screen_cell(&mut self, index: usize, value: u8) -> bool {
        if self.screen[index] == value {
            return true;
        }
        self.screen[index] = value;
        self.dirty();
        true
    }

    fn set_color_cell(&mut self, index: usize, value: u8) -> bool {
        if self.color[index] == value {
            return true;
        }
        self.color[index] = value;
        self.dirty();
        true
    }

    fn set_attr_cell(&mut self, index: usize, value: u8) -> bool {
        if self.attr[index] == value {
            return true;
        }
        self.attr[index] = value;
        self.dirty();
        true
    }

    fn set_gfx_index(&mut self, index: usize, value: u8) -> bool {
        let next = value & 0x0F;
        if self.gfx[index] == next {
            return true;
        }
        self.gfx[index] = next;
        self.dirty();
        true
    }

    fn plot(&mut self, x: i32, y: i32, color: u8) {
        if x < 0 || x >= GFX_WIDTH as i32 || y < 0 || y >= GFX_HEIGHT as i32 {
            return;
        }
        let index = y as usize * GFX_WIDTH + x as usize;
        self.set_gfx_index(index, color);
    }

    fn clear_gfx(&mut self) {
        let mut changed = false;
        for pixel in self.gfx.iter_mut() {
            if *pixel != 0 {
                *pixel = 0;
                changed = true;
            }
        }
        if changed {
            self.dirty();
        }
    }

    fn line(&mut self, mut x0: i32, mut y0: i32, mut x1: i32, mut y1: i32, color: u8) {
        if !clip_line(&mut x0, &mut y0, &mut x1, &mut y1) {
            return;
        }

        let dx = abs_i32(x1 - x0);
        let dy = -abs_i32(y1 - y0);
        let sx = if x0 < x1 { 1 } else { -1 };
        let sy = if y0 < y1 { 1 } else { -1 };
        let mut err = dx + dy;

        loop {
            self.plot(x0, y0, color);
            if x0 == x1 && y0 == y1 {
                break;
            }
            let e2 = err * 2;
            if e2 >= dy {
                err += dy;
                x0 += sx;
            }
            if e2 <= dx {
                err += dx;
                y0 += sy;
            }
        }
    }

    fn ellipse(&mut self, cx: i32, cy: i32, rx: i32, ry: i32, color: u8) {
        if rx < 0 || ry < 0 {
            return;
        }

        if rx == ry {
            let mut x = 0;
            let mut y = rx;
            let mut d = 1 - rx;
            self.plot_octants(cx, cy, x, y, color);
            while x < y {
                if d < 0 {
                    d += 2 * x + 3;
                } else {
                    d += 2 * (x - y) + 5;
                    y -= 1;
                }
                x += 1;
                self.plot_octants(cx, cy, x, y, color);
            }
            return;
        }

        let rx2 = rx as i64 * rx as i64;
        let ry2 = ry as i64 * ry as i64;
        let two_rx2 = 2 * rx2;
        let two_ry2 = 2 * ry2;
        let mut x = 0;
        let mut y = ry;
        let mut px = 0_i64;
        let mut py = two_rx2 * y as i64;
        let mut d = ry2 - rx2 * y as i64 + rx2 / 4;

        while px < py {
            self.plot_quadrants(cx, cy, x, y, color);
            x += 1;
            px += two_ry2;
            if d < 0 {
                d += ry2 + px;
            } else {
                y -= 1;
                py -= two_rx2;
                d += ry2 + px - py;
            }
        }

        d = ry2 * (x as i64 * 2 + 1) * (x as i64 * 2 + 1) / 4
            + rx2 * (y as i64 - 1) * (y as i64 - 1)
            - rx2 * ry2;
        while y >= 0 {
            self.plot_quadrants(cx, cy, x, y, color);
            y -= 1;
            py -= two_rx2;
            if d > 0 {
                d += rx2 - py;
            } else {
                x += 1;
                px += two_ry2;
                d += rx2 - py + px;
            }
        }
    }

    fn plot_quadrants(&mut self, cx: i32, cy: i32, x: i32, y: i32, color: u8) {
        self.plot(cx + x, cy + y, color);
        self.plot(cx - x, cy + y, color);
        self.plot(cx + x, cy - y, color);
        self.plot(cx - x, cy - y, color);
    }

    fn plot_octants(&mut self, cx: i32, cy: i32, x: i32, y: i32, color: u8) {
        self.plot(cx + x, cy + y, color);
        self.plot(cx - x, cy + y, color);
        self.plot(cx + x, cy - y, color);
        self.plot(cx - x, cy - y, color);
        self.plot(cx + y, cy + x, color);
        self.plot(cx - y, cy + x, color);
        self.plot(cx + y, cy - x, color);
        self.plot(cx - y, cy - x, color);
    }

    fn rect(&mut self, x0: i32, y0: i32, x1: i32, y1: i32, color: u8) {
        self.line(x0, y0, x1, y0, color);
        self.line(x0, y1, x1, y1, color);
        self.line(x0, y0, x0, y1, color);
        self.line(x1, y0, x1, y1, color);
    }

    fn fill_rect(&mut self, x0: i32, y0: i32, x1: i32, y1: i32, color: u8) {
        let left = max_i32(min_i32(x0, x1), 0);
        let right = min_i32(max_i32(x0, x1), GFX_WIDTH as i32 - 1);
        let top = max_i32(min_i32(y0, y1), 0);
        let bottom = min_i32(max_i32(y0, y1), GFX_HEIGHT as i32 - 1);
        if left > right || top > bottom {
            return;
        }

        let mut changed = false;
        let next = color & 0x0F;
        for y in top as usize..=bottom as usize {
            let row = y * GFX_WIDTH;
            for x in left as usize..=right as usize {
                let index = row + x;
                if self.gfx[index] != next {
                    self.gfx[index] = next;
                    changed = true;
                }
            }
        }
        if changed {
            self.dirty();
        }
    }

    fn flood_fill(&mut self, x: i32, y: i32, color: u8) {
        if x < 0 || x >= GFX_WIDTH as i32 || y < 0 || y >= GFX_HEIGHT as i32 {
            return;
        }

        let target = self.gfx[y as usize * GFX_WIDTH + x as usize];
        let fill = color & 0x0F;
        if target == fill {
            return;
        }

        let mut head = 0usize;
        let mut tail = 1usize;
        self.fill_queue[0] = (y as u16) * GFX_WIDTH as u16 + x as u16;
        let mut changed = false;

        while head < tail {
            let packed = self.fill_queue[head] as usize;
            head += 1;
            let cx = packed % GFX_WIDTH;
            let cy = packed / GFX_WIDTH;
            let idx = cy * GFX_WIDTH + cx;
            if self.gfx[idx] != target {
                continue;
            }

            let mut left = cx;
            while left > 0 && self.gfx[cy * GFX_WIDTH + left - 1] == target {
                left -= 1;
            }

            let mut right = cx;
            while right < GFX_WIDTH - 1 && self.gfx[cy * GFX_WIDTH + right + 1] == target {
                right += 1;
            }

            for px in left..=right {
                let fill_index = cy * GFX_WIDTH + px;
                if self.gfx[fill_index] != fill {
                    self.gfx[fill_index] = fill;
                    changed = true;
                }
            }

            if cy > 0 {
                tail = self.enqueue_fill_spans(cy - 1, left, right, target, tail);
            }
            if cy + 1 < GFX_HEIGHT {
                tail = self.enqueue_fill_spans(cy + 1, left, right, target, tail);
            }
        }

        if changed {
            self.dirty();
        }
    }

    fn enqueue_fill_spans(
        &mut self,
        y: usize,
        left: usize,
        right: usize,
        target: u8,
        mut tail: usize,
    ) -> usize {
        let mut prev = false;
        let row = y * GFX_WIDTH;
        for x in left..=right {
            let curr = self.gfx[row + x] == target;
            if curr && !prev && tail < GFX_SIZE {
                self.fill_queue[tail] = (row + x) as u16;
                tail += 1;
            }
            prev = curr;
        }
        tail
    }

    fn read_screen_window(&self, offset: usize) -> u8 {
        if offset >= SCREEN_SIZE {
            return 0;
        }
        match self.screen_win_plane {
            SCREEN_WIN_PLANE_COLOR => self.color[offset],
            SCREEN_WIN_PLANE_ATTR => self.attr[offset],
            _ => self.screen[offset],
        }
    }

    fn write_screen_window(&mut self, offset: usize, value: u8) {
        if offset >= SCREEN_SIZE {
            return;
        }
        match self.screen_win_plane {
            SCREEN_WIN_PLANE_COLOR => self.color[offset] = value,
            SCREEN_WIN_PLANE_ATTR => self.attr[offset] = value,
            _ => {
                self.screen[offset] = value;
                self.color[offset] = self.current_color();
                self.attr[offset] = 0;
            }
        }
        self.dirty();
    }

    fn read_sprite_register(&self, addr: u16) -> u8 {
        self.sprite_regs[(addr - SPRITE_REG_BASE) as usize]
    }

    fn write_sprite_register(&mut self, addr: u16, value: u8) {
        let offset = (addr - SPRITE_REG_BASE) as usize;
        let sprite = offset / SPRITE_REG_STRIDE;
        let field = offset % SPRITE_REG_STRIDE;
        if sprite >= MAX_SPRITES {
            return;
        }
        let index = sprite * SPRITE_REG_STRIDE + field;
        let next = match field {
            SPR_REG_Y_HI => 0,
            SPR_REG_FLAGS => value & 0x83,
            SPR_REG_PRIORITY => min_u8(value, 2),
            SPR_REG_TRANS_COLOR => value & 0x0F,
            _ => value,
        };
        self.sprite_regs[index] = next;
        if field == SPR_REG_FLAGS {
            self.update_sprite_count();
        }
        self.dirty();
    }

    fn update_sprite_count(&mut self) {
        let mut count = 0u8;
        for sprite in 0..MAX_SPRITES {
            if self.sprite_regs[sprite * SPRITE_REG_STRIDE + SPR_REG_FLAGS] & SPR_FLAG_ENABLE != 0 {
                count = count.wrapping_add(1);
            }
        }
        self.vgc_regs[(REG_SPRITE_COUNT - VGC_BASE) as usize] = count;
    }

    fn sprite_shape_base(&self, sprite: usize) -> usize {
        let shape = self.sprite_regs[sprite * SPRITE_REG_STRIDE + SPR_REG_SHAPE] as usize;
        shape * SPRITE_SHAPE_SIZE
    }

    fn execute_sprite_command(&mut self, cmd: u8) {
        let sprite = self.cmd_regs[1] as usize;
        if sprite >= MAX_SPRITES {
            return;
        }
        match cmd {
            CMD_SPR_DEF => {
                let x = self.cmd_regs[2] as usize;
                let y = self.cmd_regs[3] as usize;
                if x >= 16 || y >= 16 {
                    return;
                }
                let index = self.sprite_shape_base(sprite) + y * 8 + x / 2;
                let color = self.cmd_regs[4] & 0x0F;
                if x & 1 == 0 {
                    self.sprite_shapes[index] = (self.sprite_shapes[index] & 0x0F) | (color << 4);
                } else {
                    self.sprite_shapes[index] = (self.sprite_shapes[index] & 0xF0) | color;
                }
                self.dirty();
            }
            CMD_SPR_ROW => {
                let row = self.cmd_regs[2] as usize;
                if row >= 16 {
                    return;
                }
                let base = self.sprite_shape_base(sprite) + row * 8;
                for i in 0..8 {
                    self.sprite_shapes[base + i] = self.cmd_regs[3 + i];
                }
                self.dirty();
            }
            CMD_SPR_CLR => {
                let base = self.sprite_shape_base(sprite);
                self.sprite_shapes[base..base + SPRITE_SHAPE_SIZE].fill(0);
                self.dirty();
            }
            CMD_SPR_COPY => {
                let dest = self.cmd_regs[2] as usize;
                if dest >= MAX_SPRITES {
                    return;
                }
                let src_base = self.sprite_shape_base(sprite);
                let dst_base = self.sprite_shape_base(dest);
                self.sprite_shapes
                    .copy_within(src_base..src_base + SPRITE_SHAPE_SIZE, dst_base);
                self.dirty();
            }
            CMD_SPR_POS => {
                let base = sprite * SPRITE_REG_STRIDE;
                self.sprite_regs[base + SPR_REG_X_LO] = self.cmd_regs[2];
                self.sprite_regs[base + SPR_REG_X_HI] = self.cmd_regs[3];
                self.sprite_regs[base + SPR_REG_Y_LO] = self.cmd_regs[4];
                self.sprite_regs[base + SPR_REG_Y_HI] = 0;
                self.dirty();
            }
            CMD_SPR_ENA => {
                let index = sprite * SPRITE_REG_STRIDE + SPR_REG_FLAGS;
                self.sprite_regs[index] |= SPR_FLAG_ENABLE;
                self.update_sprite_count();
                self.dirty();
            }
            CMD_SPR_DIS => {
                let index = sprite * SPRITE_REG_STRIDE + SPR_REG_FLAGS;
                self.sprite_regs[index] &= !SPR_FLAG_ENABLE;
                self.update_sprite_count();
                self.dirty();
            }
            CMD_SPR_FLIP => {
                let index = sprite * SPRITE_REG_STRIDE + SPR_REG_FLAGS;
                self.sprite_regs[index] =
                    (self.sprite_regs[index] & !0x03) | (self.cmd_regs[2] & 0x03);
                self.dirty();
            }
            CMD_SPR_PRI => {
                self.sprite_regs[sprite * SPRITE_REG_STRIDE + SPR_REG_PRIORITY] =
                    min_u8(self.cmd_regs[2], 2);
                self.dirty();
            }
            _ => {}
        }
    }

    fn read_vgc_irq_register(&self, addr: u16) -> u8 {
        match addr {
            REG_IRQ_ENABLE => self.irq_enable,
            REG_IRQ_STATUS => self.irq_status,
            REG_IRQ_VALID => IRQ_VALID_MASK,
            _ => 0,
        }
    }

    fn write_vgc_irq_register(&mut self, addr: u16, value: u8) {
        match addr {
            REG_IRQ_ENABLE => self.irq_enable = value & IRQ_VALID_MASK,
            REG_IRQ_STATUS => self.irq_status &= !(value & IRQ_VALID_MASK),
            REG_IRQ_FORCE => self.raise_vgc_irq(value),
            _ => {}
        }
        self.update_irq_line();
    }

    fn raise_vgc_irq(&mut self, mask: u8) {
        let pending = mask & self.irq_enable & IRQ_VALID_MASK;
        if pending != 0 {
            self.irq_status |= pending;
            self.update_irq_line();
        }
    }

    fn update_irq_line(&mut self) {
        let vgc = (self.irq_status & self.irq_enable & IRQ_VALID_MASK) != 0;
        let timer = self.timer_regs[(TIMER_STATUS - TIMER_BASE) as usize] & 0x01 != 0;
        let board = (self.board_regs[(BOARD_INPUT_IRQ_STATUS - BOARD_INPUT_BASE) as usize]
            & self.board_regs[(BOARD_INPUT_IRQ_ENABLE - BOARD_INPUT_BASE) as usize]
            & BOARD_INPUT_IRQ_MASK)
            != 0;
        self.irq_waiting = vgc || timer || board;
    }

    fn advance_hardware(&mut self, cycles: u32) {
        if cycles == 0 || self.timer_regs[(TIMER_CTRL - TIMER_BASE) as usize] & 0x01 == 0 {
            return;
        }
        let divisor = self.timer_regs[(TIMER_DIV_L - TIMER_BASE) as usize] as i32
            | ((self.timer_regs[(TIMER_DIV_H - TIMER_BASE) as usize] as i32) << 8);
        if divisor <= 0 {
            return;
        }
        self.timer_cycle_accumulator += cycles as i32;
        let ticks = self.timer_cycle_accumulator / TIMER_TICK_QUANTUM_CYCLES;
        if ticks <= 0 {
            return;
        }
        self.timer_cycle_accumulator -= ticks * TIMER_TICK_QUANTUM_CYCLES;
        self.timer_counter += ticks;
        if self.timer_counter >= divisor {
            self.timer_counter %= divisor;
            self.timer_regs[(TIMER_STATUS - TIMER_BASE) as usize] = 0x01;
            self.update_irq_line();
        }
    }

    fn read_timer_register(&mut self, addr: u16) -> u8 {
        let index = (addr - TIMER_BASE) as usize;
        if addr == TIMER_STATUS {
            let result = self.timer_regs[index] & 0x01;
            self.timer_regs[index] = 0;
            self.update_irq_line();
            return result;
        }
        self.timer_regs[index]
    }

    fn write_timer_register(&mut self, addr: u16, value: u8) {
        let index = (addr - TIMER_BASE) as usize;
        self.timer_regs[index] = value;
        if addr == TIMER_CTRL && value & 0x01 == 0 {
            self.timer_counter = 0;
            self.timer_cycle_accumulator = 0;
            self.timer_regs[(TIMER_STATUS - TIMER_BASE) as usize] = 0;
        }
        self.update_irq_line();
    }

    fn read_board_register(&self, addr: u16) -> u8 {
        self.board_regs[(addr - BOARD_INPUT_BASE) as usize]
    }

    fn write_board_register(&mut self, addr: u16, value: u8) {
        let index = (addr - BOARD_INPUT_BASE) as usize;
        match addr {
            BOARD_INPUT_IRQ_ENABLE => self.board_regs[index] = value & BOARD_INPUT_IRQ_MASK,
            BOARD_INPUT_IRQ_STATUS => self.board_regs[index] &= !(value & BOARD_INPUT_IRQ_MASK),
            BOARD_INPUT_BUTTON_CHANGES | BOARD_INPUT_SWITCH_CHANGES => {
                self.board_regs[index] &= !value
            }
            BOARD_INPUT_BUTTONS | BOARD_INPUT_SWITCHES => {}
            _ => self.board_regs[index] = value,
        }
        self.update_irq_line();
    }

    fn read_nic_register(&mut self, addr: u16) -> u8 {
        match addr {
            NIC_STATUS => NIC_STATUS_READY,
            NIC_IRQ_STATUS => {
                let value = self.nic_regs[(addr - NIC_BASE) as usize];
                self.nic_regs[(addr - NIC_BASE) as usize] = 0;
                value
            }
            NIC_DMA_STATUS => NIC_DMA_STATUS_TX_READY,
            NIC_SLOT_STATUS0..=NIC_SLOT_STATUS3 => NIC_SLOT_SEND_READY,
            _ => self.nic_regs[(addr - NIC_BASE) as usize],
        }
    }

    fn write_nic_register(&mut self, addr: u16, value: u8) {
        let index = (addr - NIC_BASE) as usize;
        self.nic_regs[index] = value;
        if addr == NIC_IRQ_STATUS {
            self.nic_regs[index] = 0;
        }
    }

    fn read_wts_register(&self, addr: u16) -> u8 {
        match addr {
            WTS_SOUNDFONT_STATUS => 0,
            WTS_EVENT_STATUS => self.wts_regs[(addr - WTS_BASE) as usize] | 0x05,
            _ => self.wts_regs[(addr - WTS_BASE) as usize],
        }
    }

    fn write_wts_register(&mut self, addr: u16, value: u8) {
        self.wts_regs[(addr - WTS_BASE) as usize] = value;
    }

    fn read_dma_reg24(&self, base: u16) -> usize {
        let index = (base - DMA_BASE) as usize;
        self.dma_regs[index] as usize
            | ((self.dma_regs[index + 1] as usize) << 8)
            | ((self.dma_regs[index + 2] as usize) << 16)
    }

    fn write_dma_count(&mut self, count: usize) {
        let index = (DMA_COUNT_L - DMA_BASE) as usize;
        self.dma_regs[index] = count as u8;
        self.dma_regs[index + 1] = (count >> 8) as u8;
        self.dma_regs[index + 2] = (count >> 16) as u8;
    }

    fn write_dma_status(&mut self, status: u8, err: u8) {
        self.dma_regs[(DMA_STATUS - DMA_BASE) as usize] = status;
        self.dma_regs[(DMA_ERRCODE - DMA_BASE) as usize] = err;
    }

    fn write_dma_register(&mut self, addr: u16, value: u8) {
        self.dma_regs[(addr - DMA_BASE) as usize] = value;
        if addr == DMA_CMD {
            self.execute_dma(value);
        }
    }

    fn execute_dma(&mut self, command: u8) {
        if command != DMA_CMD_START {
            self.write_dma_status(DMA_STATUS_ERROR, DMA_ERR_BAD_CMD);
            return;
        }
        let len = self.read_dma_reg24(DMA_LEN_L);
        if len == 0 {
            self.write_dma_count(0);
            self.write_dma_status(DMA_STATUS_ERROR, DMA_ERR_BAD_ARGS);
            return;
        }
        let src_space = self.dma_regs[(DMA_SRC_SPACE - DMA_BASE) as usize];
        let dst_space = self.dma_regs[(DMA_DST_SPACE - DMA_BASE) as usize];
        let src = self.read_dma_reg24(DMA_SRC_L);
        let dst = self.read_dma_reg24(DMA_DST_L);
        let fill = self.dma_regs[(DMA_MODE - DMA_BASE) as usize] & DMA_MODE_FILL != 0;
        let fill_value = self.dma_regs[(DMA_FILL_VALUE - DMA_BASE) as usize];
        if (!fill && !self.dma_range_ok(src_space, src, len))
            || !self.dma_range_ok(dst_space, dst, len)
        {
            self.write_dma_count(0);
            self.write_dma_status(DMA_STATUS_ERROR, DMA_ERR_RANGE);
            return;
        }
        if !self.dma_can_write_range(dst_space, dst, len) {
            self.write_dma_count(0);
            self.write_dma_status(DMA_STATUS_ERROR, DMA_ERR_WRITE_PROT);
            return;
        }
        for i in 0..len {
            let value = if fill {
                fill_value
            } else if let Some(value) = self.dma_read(src_space, src + i) {
                value
            } else {
                self.write_dma_count(i);
                self.write_dma_status(DMA_STATUS_ERROR, DMA_ERR_RANGE);
                return;
            };
            if !self.dma_write(dst_space, dst + i, value) {
                self.write_dma_count(i);
                self.write_dma_status(DMA_STATUS_ERROR, DMA_ERR_RANGE);
                return;
            }
        }
        self.write_dma_count(len);
        self.write_dma_status(DMA_STATUS_OK, DMA_ERR_NONE);
        self.dirty();
    }

    fn blt_index(addr: u16) -> usize {
        if addr == BLT_ROTATE_ANGLE {
            (BLT_END - BLT_BASE + 1) as usize
        } else {
            (addr - BLT_BASE) as usize
        }
    }

    fn read_blt_register(&self, addr: u16) -> u8 {
        self.blt_regs[Self::blt_index(addr)]
    }

    fn write_blt_register(&mut self, addr: u16, value: u8) {
        let index = Self::blt_index(addr);
        self.blt_regs[index] = value;
        if addr == BLT_CMD {
            self.execute_blt(value);
        }
    }

    fn blt_u16(&self, base: u16) -> usize {
        let index = Self::blt_index(base);
        self.blt_regs[index] as usize | ((self.blt_regs[index + 1] as usize) << 8)
    }

    fn blt_u24(&self, base: u16) -> usize {
        let index = Self::blt_index(base);
        self.blt_regs[index] as usize
            | ((self.blt_regs[index + 1] as usize) << 8)
            | ((self.blt_regs[index + 2] as usize) << 16)
    }

    fn write_blt_count(&mut self, count: usize) {
        let index = Self::blt_index(BLT_COUNT_L);
        self.blt_regs[index] = count as u8;
        self.blt_regs[index + 1] = (count >> 8) as u8;
        self.blt_regs[index + 2] = (count >> 16) as u8;
    }

    fn write_blt_status(&mut self, status: u8, err: u8) {
        self.blt_regs[Self::blt_index(BLT_STATUS)] = status;
        self.blt_regs[Self::blt_index(BLT_ERRCODE)] = err;
    }

    fn execute_blt(&mut self, command: u8) {
        if command != BLT_CMD_START {
            self.write_blt_status(BLT_STATUS_ERROR, DMA_ERR_BAD_CMD);
            return;
        }
        let width = self.blt_u16(BLT_WIDTH_L);
        let height = self.blt_u16(BLT_HEIGHT_L);
        if width == 0 || height == 0 {
            self.write_blt_count(0);
            self.write_blt_status(BLT_STATUS_ERROR, DMA_ERR_BAD_ARGS);
            return;
        }
        let src_space = self.blt_regs[Self::blt_index(BLT_SRC_SPACE)];
        let dst_space = self.blt_regs[Self::blt_index(BLT_DST_SPACE)];
        let src_base = self.blt_u24(BLT_SRC_L);
        let dst_base = self.blt_u24(BLT_DST_L);
        let src_stride = self.blt_u16(BLT_SRC_STRIDE_L);
        let dst_stride = self.blt_u16(BLT_DST_STRIDE_L);
        let mode = self.blt_regs[Self::blt_index(BLT_MODE)];
        let fill = mode & BLT_MODE_FILL != 0;
        let rotate = mode & BLT_MODE_ROTATE != 0;
        let color_key = self.blt_regs[Self::blt_index(BLT_COLOR_KEY)];
        let fill_value = self.blt_regs[Self::blt_index(BLT_FILL_VALUE)];
        if dst_stride == 0 || (!fill && src_stride == 0) {
            self.write_blt_count(0);
            self.write_blt_status(BLT_STATUS_ERROR, DMA_ERR_BAD_ARGS);
            return;
        }
        if rotate && (fill || width != height || width > 256) {
            self.write_blt_count(0);
            self.write_blt_status(BLT_STATUS_ERROR, DMA_ERR_BAD_ARGS);
            return;
        }
        if rotate
            && src_space == dst_space
            && rect_linear_ranges_overlap(
                src_base, width, height, src_stride, dst_base, width, height, dst_stride,
            )
        {
            self.write_blt_count(0);
            self.write_blt_status(BLT_STATUS_ERROR, DMA_ERR_BAD_ARGS);
            return;
        }
        if !self.dma_range_ok(dst_space, dst_base, rect_extent(width, height, dst_stride))
            || !self.dma_can_write_rect(dst_space, dst_base, width, height, dst_stride)
        {
            self.write_blt_count(0);
            self.write_blt_status(BLT_STATUS_ERROR, DMA_ERR_RANGE);
            return;
        }
        if !fill && !self.dma_range_ok(src_space, src_base, rect_extent(width, height, src_stride))
        {
            self.write_blt_count(0);
            self.write_blt_status(BLT_STATUS_ERROR, DMA_ERR_RANGE);
            return;
        }
        let mut written = 0usize;
        let angle = self.blt_regs[Self::blt_index(BLT_ROTATE_ANGLE)];
        for row in 0..height {
            let dst_row = dst_base + row * dst_stride;
            for col in 0..width {
                let value = if fill {
                    fill_value
                } else if rotate {
                    match rotated_source_offset(width, row, col, angle) {
                        Some((src_row, src_col)) => match self
                            .dma_read(src_space, src_base + src_row * src_stride + src_col)
                        {
                            Some(value) => value,
                            None => {
                                self.write_blt_count(written);
                                self.write_blt_status(BLT_STATUS_ERROR, DMA_ERR_RANGE);
                                return;
                            }
                        },
                        None => color_key,
                    }
                } else {
                    let src_row = src_base + row * src_stride;
                    match self.dma_read(src_space, src_row + col) {
                        Some(value) => value,
                        None => {
                            self.write_blt_count(written);
                            self.write_blt_status(BLT_STATUS_ERROR, DMA_ERR_RANGE);
                            return;
                        }
                    }
                };
                if mode & BLT_MODE_COLOR_KEY != 0 && value == color_key {
                    continue;
                }
                if !self.dma_write(dst_space, dst_row + col, value) {
                    self.write_blt_count(written);
                    self.write_blt_status(BLT_STATUS_ERROR, DMA_ERR_RANGE);
                    return;
                }
                written += 1;
            }
        }
        self.write_blt_count(written);
        self.write_blt_status(BLT_STATUS_OK, DMA_ERR_NONE);
        self.dirty();
    }

    fn dma_can_write_rect(
        &self,
        space: u8,
        base: usize,
        width: usize,
        height: usize,
        stride: usize,
    ) -> bool {
        for row in 0..height {
            if !self.dma_can_write_range(space, base + row * stride, width) {
                return false;
            }
        }
        true
    }

    fn dma_space_len(&self, space: u8) -> usize {
        match space {
            DMA_SPACE_CPU_RAM => RAM_SIZE,
            DMA_SPACE_VGC_CHAR | DMA_SPACE_VGC_COLOR | DMA_SPACE_VGC_TEXT_ATTR => SCREEN_SIZE,
            DMA_SPACE_VGC_GFX => GFX_SIZE,
            DMA_SPACE_VGC_SPRITE => SPRITE_SHAPE_RAM_SIZE,
            DMA_SPACE_XRAM => XRAM_SIZE,
            _ => 0,
        }
    }

    fn dma_range_ok(&self, space: u8, address: usize, length: usize) -> bool {
        let len = self.dma_space_len(space);
        len != 0 && address <= len && length <= len && address.saturating_add(length) <= len
    }

    fn dma_can_write_range(&self, space: u8, address: usize, length: usize) -> bool {
        if space != DMA_SPACE_CPU_RAM {
            return true;
        }
        let end = address.saturating_add(length);
        end <= ROM_BASE as usize || (address >= 0xFFFA && end <= RAM_SIZE)
    }

    fn dma_read(&self, space: u8, address: usize) -> Option<u8> {
        match space {
            DMA_SPACE_CPU_RAM if address < RAM_SIZE => Some(self.ram[address]),
            DMA_SPACE_VGC_CHAR if address < SCREEN_SIZE => Some(self.screen[address]),
            DMA_SPACE_VGC_COLOR if address < SCREEN_SIZE => Some(self.color[address]),
            DMA_SPACE_VGC_TEXT_ATTR if address < SCREEN_SIZE => Some(self.attr[address]),
            DMA_SPACE_VGC_GFX if address < GFX_SIZE => Some(self.gfx[address]),
            DMA_SPACE_VGC_SPRITE if address < SPRITE_SHAPE_RAM_SIZE => {
                Some(self.sprite_shapes[address])
            }
            DMA_SPACE_XRAM if address < XRAM_SIZE => Some(self.xram[address]),
            _ => None,
        }
    }

    fn dma_write(&mut self, space: u8, address: usize, value: u8) -> bool {
        match space {
            DMA_SPACE_CPU_RAM
                if address < RAM_SIZE && self.dma_can_write_range(space, address, 1) =>
            {
                self.ram[address] = value;
                true
            }
            DMA_SPACE_VGC_CHAR if address < SCREEN_SIZE => self.set_screen_cell(address, value),
            DMA_SPACE_VGC_COLOR if address < SCREEN_SIZE => self.set_color_cell(address, value),
            DMA_SPACE_VGC_TEXT_ATTR if address < SCREEN_SIZE => self.set_attr_cell(address, value),
            DMA_SPACE_VGC_GFX if address < GFX_SIZE => self.set_gfx_index(address, value),
            DMA_SPACE_VGC_SPRITE if address < SPRITE_SHAPE_RAM_SIZE => {
                self.sprite_shapes[address] = value;
                self.dirty();
                true
            }
            DMA_SPACE_XRAM if address < XRAM_SIZE => {
                self.xram[address] = value;
                true
            }
            _ => false,
        }
    }

    fn read_math_register(&mut self, addr: u16) -> u8 {
        if addr == MATH_RNG {
            self.math_rng_state = next_rng(self.math_rng_state);
            self.math_regs[(MATH_STATUS - MATH_BASE) as usize] = MATH_STATUS_OK;
            return self.math_rng_state as u8;
        }
        match addr {
            MATH_CAPS0 => MATH_CAPS0_VALUE,
            MATH_CAPS1 => MATH_CAPS1_VALUE,
            MATH_VERSION => MATH_VERSION_CURRENT,
            _ => self.math_regs[(addr - MATH_BASE) as usize],
        }
    }

    fn write_math_register(&mut self, addr: u16, value: u8) {
        match addr {
            MATH_CAPS0 | MATH_CAPS1 | MATH_STATUS | MATH_VERSION => return,
            _ => self.math_regs[(addr - MATH_BASE) as usize] = value,
        }
        match addr {
            MATH_MUL16_B_HI => self.math_mul16(),
            MATH_MULFX_B_HI => self.math_mulfx(),
            MATH_DIV_D_HI => self.math_div_s32_s16(),
            MATH_SINCOS_ANGLE => self.math_sincos(value),
            MATH_ATAN_DX_HI => self.math_atan2(),
            MATH_DIST_DY_HI => self.math_dist(),
            MATH_RNG => self.seed_math_rng(value),
            MATH_VEC_OP => self.math_vector_op(value),
            _ => {}
        }
    }

    fn math_i16(&self, addr: u16) -> i16 {
        let index = (addr - MATH_BASE) as usize;
        (self.math_regs[index] as u16 | ((self.math_regs[index + 1] as u16) << 8)) as i16
    }

    fn math_i32(&self, addr: u16) -> i32 {
        let index = (addr - MATH_BASE) as usize;
        self.math_regs[index] as i32
            | ((self.math_regs[index + 1] as i32) << 8)
            | ((self.math_regs[index + 2] as i32) << 16)
            | ((self.math_regs[index + 3] as i32) << 24)
    }

    fn math_set_result32(&mut self, value: i32, status: u8) {
        let index = (MATH_RES0 - MATH_BASE) as usize;
        let raw = value as u32;
        self.math_regs[index] = raw as u8;
        self.math_regs[index + 1] = (raw >> 8) as u8;
        self.math_regs[index + 2] = (raw >> 16) as u8;
        self.math_regs[index + 3] = (raw >> 24) as u8;
        self.math_regs[(MATH_STATUS - MATH_BASE) as usize] = status;
    }

    fn math_set_result16(&mut self, value: u16, status: u8) {
        let index = (MATH_RES0 - MATH_BASE) as usize;
        self.math_regs[index] = value as u8;
        self.math_regs[index + 1] = (value >> 8) as u8;
        self.math_regs[index + 2] = 0;
        self.math_regs[index + 3] = 0;
        self.math_regs[(MATH_STATUS - MATH_BASE) as usize] = status;
    }

    fn math_set_words(&mut self, lo: u16, hi: u16, status: u8) {
        let index = (MATH_RES0 - MATH_BASE) as usize;
        self.math_regs[index] = lo as u8;
        self.math_regs[index + 1] = (lo >> 8) as u8;
        self.math_regs[index + 2] = hi as u8;
        self.math_regs[index + 3] = (hi >> 8) as u8;
        self.math_regs[(MATH_STATUS - MATH_BASE) as usize] = status;
    }

    fn math_mul16(&mut self) {
        let a = self.math_i16(0xBB20) as i32;
        let b = self.math_i16(0xBB22) as i32;
        self.math_set_result32(a.wrapping_mul(b), MATH_STATUS_OK);
    }

    fn math_mulfx(&mut self) {
        let a = self.math_i16(0xBB24) as i32;
        let b = self.math_i16(0xBB26) as i32;
        let shifted = (a * b) >> 8;
        let (value, status) = saturate_i16(shifted as i64);
        self.math_set_result16(value, status);
    }

    fn math_div_s32_s16(&mut self) {
        let n = self.math_i32(0xBB28);
        let d = self.math_i16(0xBB2C) as i32;
        if d == 0 {
            self.math_set_result32(-1, MATH_STATUS_DIV_ZERO);
            return;
        }
        let mut q = n as i64 / d as i64;
        let mut r = n as i64 % d as i64;
        let mut status = MATH_STATUS_OK;
        if q > i16::MAX as i64 {
            q = i16::MAX as i64;
            r = 0;
            status = MATH_STATUS_OVERFLOW;
        } else if q < i16::MIN as i64 {
            q = i16::MIN as i64;
            r = 0;
            status = MATH_STATUS_OVERFLOW;
        }
        self.math_set_words(q as i16 as u16, r as i16 as u16, status);
    }

    fn math_sincos(&mut self, angle: u8) {
        let index = (MATH_RES0 - MATH_BASE) as usize;
        self.math_regs[index] = sin1p7(angle);
        self.math_regs[index + 1] = sin1p7(angle.wrapping_add(64));
        self.math_regs[index + 2] = 0;
        self.math_regs[index + 3] = 0;
        self.math_regs[(MATH_STATUS - MATH_BASE) as usize] = MATH_STATUS_OK;
    }

    fn math_dist(&mut self) {
        let dx = self.math_i16(0xBB33);
        let dy = self.math_i16(0xBB35);
        self.math_set_result16(dist_approx(dx, dy), MATH_STATUS_OK);
    }

    fn math_atan2(&mut self) {
        let dy = self.math_i16(0xBB2F);
        let dx = self.math_i16(0xBB31);
        let hypot = dist_approx(dx, dy);
        let angle = atan2_angle(dy, dx);
        let index = (MATH_RES0 - MATH_BASE) as usize;
        self.math_regs[index] = angle;
        self.math_regs[index + 1] = hypot as u8;
        self.math_regs[index + 2] = (hypot >> 8) as u8;
        self.math_regs[index + 3] = 0;
        self.math_regs[(MATH_STATUS - MATH_BASE) as usize] = MATH_STATUS_OK;
    }

    fn math_vector_op(&mut self, op: u8) {
        let ax = self.math_i16(0xBB40) as i64;
        let ay = self.math_i16(0xBB42) as i64;
        let bx = self.math_i16(0xBB44) as i64;
        let by = self.math_i16(0xBB46) as i64;
        let scalar = self.math_i16(0xBB48) as i64;
        match op {
            0x01 => self.math_set_saturating32(ax * bx + ay * by),
            0x02 => {
                let (value, status) = saturate_i16((ax * bx + ay * by) >> 8);
                self.math_set_result16(value, status);
            }
            0x03 => self.math_set_saturating32(ax * by - ay * bx),
            0x04 => self.math_set_result32((ax * ax + ay * ay) as i32, MATH_STATUS_OK),
            0x05 => {
                let (sx, sx_status) = saturate_i16((ax * scalar) >> 8);
                let (sy, sy_status) = saturate_i16((ay * scalar) >> 8);
                let status = if sx_status == MATH_STATUS_OK && sy_status == MATH_STATUS_OK {
                    MATH_STATUS_OK
                } else {
                    MATH_STATUS_OVERFLOW
                };
                self.math_set_words(sx, sy, status);
            }
            _ => self.math_set_result32(-1, MATH_STATUS_UNIMPL),
        }
    }

    fn math_set_saturating32(&mut self, value: i64) {
        if value > i32::MAX as i64 {
            self.math_set_result32(i32::MAX, MATH_STATUS_OVERFLOW);
        } else if value < i32::MIN as i64 {
            self.math_set_result32(i32::MIN, MATH_STATUS_OVERFLOW);
        } else {
            self.math_set_result32(value as i32, MATH_STATUS_OK);
        }
    }

    fn seed_math_rng(&mut self, value: u8) {
        let shift = (self.math_rng_seed_index as u32) * 8;
        self.math_rng_state =
            (self.math_rng_state & !(0xFFu32 << shift)) | ((value as u32) << shift);
        if self.math_rng_state == 0 {
            self.math_rng_state = 0xDEADBEEF;
        }
        self.math_rng_seed_index = (self.math_rng_seed_index + 1) & 0x03;
        self.math_regs[(MATH_STATUS - MATH_BASE) as usize] = MATH_STATUS_OK;
    }

    fn read_xmc_register(&self, addr: u16) -> u8 {
        self.xmc_regs[(addr - XMC_BASE) as usize]
    }

    fn write_xmc_register(&mut self, addr: u16, value: u8) {
        let index = (addr - XMC_BASE) as usize;
        match addr {
            XMC_STATUS | XMC_ERRCODE | XMC_BANKS => {}
            XMC_CMD => self.execute_xmc_command(value),
            _ => self.xmc_regs[index] = value,
        }
    }

    fn execute_xmc_command(&mut self, command: u8) {
        self.xmc_regs[(XMC_CMD - XMC_BASE) as usize] = command;
        match command {
            0 => {}
            XMC_CMD_GET => {
                let addr = self.xmc_address();
                if !self.xmc_range_ok(addr, 1) {
                    self.set_xmc_status(XMC_STATUS_ERROR, XMC_ERR_RANGE);
                    return;
                }
                self.xmc_regs[(XMC_DATA - XMC_BASE) as usize] = self.xram[addr];
                self.set_xmc_status(XMC_STATUS_OK, XMC_ERR_NONE);
            }
            XMC_CMD_PUT => {
                let addr = self.xmc_address();
                if !self.xmc_range_ok(addr, 1) {
                    self.set_xmc_status(XMC_STATUS_ERROR, XMC_ERR_RANGE);
                    return;
                }
                self.xram[addr] = self.xmc_regs[(XMC_DATA - XMC_BASE) as usize];
                self.set_xmc_status(XMC_STATUS_OK, XMC_ERR_NONE);
            }
            XMC_CMD_STASH => {
                let xaddr = self.xmc_address();
                let raddr = self.xmc_ram_address();
                let len = self.xmc_length();
                if len == 0 || !self.xmc_range_ok(xaddr, len) || raddr + len > RAM_SIZE {
                    self.set_xmc_status(XMC_STATUS_ERROR, XMC_ERR_RANGE);
                    return;
                }
                self.xram[xaddr..xaddr + len].copy_from_slice(&self.ram[raddr..raddr + len]);
                self.set_xmc_status(XMC_STATUS_OK, XMC_ERR_NONE);
            }
            XMC_CMD_FETCH => {
                let xaddr = self.xmc_address();
                let raddr = self.xmc_ram_address();
                let len = self.xmc_length();
                if len == 0 || !self.xmc_range_ok(xaddr, len) || raddr + len > RAM_SIZE {
                    self.set_xmc_status(XMC_STATUS_ERROR, XMC_ERR_RANGE);
                    return;
                }
                self.ram[raddr..raddr + len].copy_from_slice(&self.xram[xaddr..xaddr + len]);
                self.set_xmc_status(XMC_STATUS_OK, XMC_ERR_NONE);
            }
            XMC_CMD_FILL => {
                let xaddr = self.xmc_address();
                let len = self.xmc_length();
                if len == 0 || !self.xmc_range_ok(xaddr, len) {
                    self.set_xmc_status(XMC_STATUS_ERROR, XMC_ERR_RANGE);
                    return;
                }
                let value = self.xmc_regs[(XMC_DATA - XMC_BASE) as usize];
                for item in self.xram[xaddr..xaddr + len].iter_mut() {
                    *item = value;
                }
                self.set_xmc_status(XMC_STATUS_OK, XMC_ERR_NONE);
            }
            XMC_CMD_STATS => self.set_xmc_status(XMC_STATUS_OK, XMC_ERR_NONE),
            _ => self.set_xmc_status(XMC_STATUS_ERROR, XMC_ERR_BADARGS),
        }
    }

    fn read_xmc_window(&self, addr: u16) -> u8 {
        let window = ((addr - XMC_WINDOW_BASE) >> 8) as usize;
        if !self.xmc_window_enabled(window) {
            return self.ram[addr as usize];
        }
        let xaddr = (self.xmc_window_base(window) & !0xFF) + ((addr & 0xFF) as usize);
        if !self.xmc_range_ok(xaddr, 1) {
            return 0;
        }
        self.xram[xaddr]
    }

    fn write_xmc_window(&mut self, addr: u16, value: u8) {
        let window = ((addr - XMC_WINDOW_BASE) >> 8) as usize;
        if !self.xmc_window_enabled(window) {
            self.ram[addr as usize] = value;
            return;
        }
        let xaddr = (self.xmc_window_base(window) & !0xFF) + ((addr & 0xFF) as usize);
        if self.xmc_range_ok(xaddr, 1) {
            self.xram[xaddr] = value;
        }
    }

    fn xmc_address(&self) -> usize {
        self.xmc_regs[(XMC_XAL - XMC_BASE) as usize] as usize
            | ((self.xmc_regs[(XMC_XAM - XMC_BASE) as usize] as usize) << 8)
            | ((self.xmc_regs[(XMC_XAH - XMC_BASE) as usize] as usize) << 16)
    }

    fn xmc_ram_address(&self) -> usize {
        self.xmc_regs[(XMC_RAML - XMC_BASE) as usize] as usize
            | ((self.xmc_regs[(XMC_RAMH - XMC_BASE) as usize] as usize) << 8)
    }

    fn xmc_length(&self) -> usize {
        self.xmc_regs[(XMC_LENL - XMC_BASE) as usize] as usize
            | ((self.xmc_regs[(XMC_LENH - XMC_BASE) as usize] as usize) << 8)
    }

    fn xmc_window_enabled(&self, window: usize) -> bool {
        window < 4 && (self.xmc_regs[(XMC_WINCTL - XMC_BASE) as usize] & (1 << window)) != 0
    }

    fn xmc_window_base(&self, window: usize) -> usize {
        let base = (XMC_WIN0AL - XMC_BASE) as usize + window * 3;
        self.xmc_regs[base] as usize
            | ((self.xmc_regs[base + 1] as usize) << 8)
            | ((self.xmc_regs[base + 2] as usize) << 16)
    }

    fn xmc_range_ok(&self, address: usize, length: usize) -> bool {
        address <= XRAM_SIZE && length <= XRAM_SIZE && address.saturating_add(length) <= XRAM_SIZE
    }

    fn set_xmc_status(&mut self, status: u8, err: u8) {
        self.xmc_regs[(XMC_STATUS - XMC_BASE) as usize] = status;
        self.xmc_regs[(XMC_ERRCODE - XMC_BASE) as usize] = err;
    }

    fn write_fio(&mut self, addr: u16, value: u8) {
        self.ram[addr as usize] = value;
        if addr != FIO_CMD {
            return;
        }

        match value {
            0 => {}
            FIO_CMD_RNG => {
                self.ram[FIO_SRC_L as usize] = 0x4D;
                self.ram[FIO_SRC_H as usize] = 0x21;
                self.ram[FIO_END_L as usize] = 0xAF;
                self.ram[FIO_END_H as usize] = 0x13;
                self.ram[FIO_ERRCODE as usize] = 0;
                self.ram[FIO_STATUS as usize] = FIO_STATUS_OK;
            }
            FIO_CMD_LOAD_MODULE => {
                self.load_module_from_store();
            }
            FIO_CMD_CLEARERR => {
                self.pending_fio_cmd = 0;
                self.ram[FIO_ERRCODE as usize] = 0;
                self.ram[FIO_STATUS as usize] = FIO_STATUS_OK;
                self.ram[FIO_CMD as usize] = 0;
            }
            _ if is_host_fio_command(value) => {
                self.pending_fio_cmd = value;
                self.ram[FIO_ERRCODE as usize] = 0;
                self.ram[FIO_STATUS as usize] = 0;
            }
            _ => {
                self.ram[FIO_ERRCODE as usize] = 0;
                self.ram[FIO_STATUS as usize] = FIO_STATUS_OK;
                self.ram[FIO_CMD as usize] = 0;
            }
        }
    }

    fn load_module_from_store(&mut self) {
        let id = self.ram[FIO_SRC_L as usize];
        let slot = self.ram[FIO_END_L as usize] as usize;
        if slot >= MODULE_SLOTS {
            self.ram[FIO_ERRCODE as usize] = 1;
            self.ram[FIO_STATUS as usize] = FIO_STATUS_ERROR;
            return;
        }

        let mut store_slot = MODULE_STORE_SLOTS;
        for i in 0..MODULE_STORE_SLOTS {
            if MODULE_STORE_IDS[i] == id {
                store_slot = i;
                break;
            }
        }

        if store_slot >= MODULE_STORE_SLOTS {
            self.ram[FIO_ERRCODE as usize] = 1;
            self.ram[FIO_STATUS as usize] = FIO_STATUS_ERROR;
            return;
        }

        let dest = SHELF_BASE + slot * MODULE_SLOT_BYTES;
        unsafe {
            core::ptr::copy_nonoverlapping(
                addr_of!(MODULE_BUFFERS)
                    .cast::<u8>()
                    .add(store_slot * MODULE_SLOT_BYTES),
                self.xram.as_mut_ptr().add(dest),
                MODULE_SLOT_BYTES,
            );
        }
        self.ram[FIO_SRC_L as usize] = (MODULE_SLOT_BYTES & 0xFF) as u8;
        self.ram[FIO_SRC_H as usize] = ((MODULE_SLOT_BYTES >> 8) & 0xFF) as u8;
        self.ram[FIO_ERRCODE as usize] = 0;
        self.ram[FIO_STATUS as usize] = FIO_STATUS_OK;
    }

    fn char_out(&mut self, data: u8) {
        let mut cx = min_usize(
            self.vgc_regs[(REG_CURSOR_X - VGC_BASE) as usize] as usize,
            SCREEN_COLS - 1,
        );
        let mut cy = min_usize(
            self.vgc_regs[(REG_CURSOR_Y - VGC_BASE) as usize] as usize,
            SCREEN_ROWS - 1,
        );
        match data {
            0x08 => {
                if cx > 0 {
                    cx -= 1;
                }
                self.screen[cy * SCREEN_COLS + cx] = 0x20;
            }
            0x0A => {
                cy += 1;
                if cy >= SCREEN_ROWS {
                    self.scroll_up();
                    cy = SCREEN_ROWS - 1;
                }
            }
            0x0C => {
                self.screen = [0x20; SCREEN_SIZE];
                let color = self.current_color();
                self.color = [color; SCREEN_SIZE];
                self.attr = [0; SCREEN_SIZE];
                cx = 0;
                cy = 0;
            }
            0x0D => cx = 0,
            0x13 => {
                cx = 0;
                cy = 0;
            }
            _ if data >= 0x20 => {
                let font = self.vgc_regs[(REG_FONT - VGC_BASE) as usize];
                let mut ch = data;
                if font == 1 && (b'a'..=b'z').contains(&ch) {
                    ch -= 0x20;
                }
                let idx = cy * SCREEN_COLS + cx;
                self.screen[idx] = ch;
                self.color[idx] = self.current_color();
                self.attr[idx] = 0;
                cx += 1;
                if cx >= SCREEN_COLS {
                    cx = 0;
                    cy += 1;
                    if cy >= SCREEN_ROWS {
                        self.scroll_up();
                        cy = SCREEN_ROWS - 1;
                    }
                }
            }
            _ => {}
        }

        self.vgc_regs[(REG_CURSOR_X - VGC_BASE) as usize] = cx as u8;
        self.vgc_regs[(REG_CURSOR_Y - VGC_BASE) as usize] = cy as u8;
        self.dirty();
    }

    fn scroll_up(&mut self) {
        self.screen.copy_within(SCREEN_COLS..SCREEN_SIZE, 0);
        self.color.copy_within(SCREEN_COLS..SCREEN_SIZE, 0);
        self.attr.copy_within(SCREEN_COLS..SCREEN_SIZE, 0);
        let start = (SCREEN_ROWS - 1) * SCREEN_COLS;
        let color = self.current_color();
        for i in start..SCREEN_SIZE {
            self.screen[i] = 0x20;
            self.color[i] = color;
            self.attr[i] = 0;
        }
    }

    fn current_color(&self) -> u8 {
        let fg = self.vgc_regs[(REG_FG - VGC_BASE) as usize] & 0x0F;
        let bg = self.vgc_regs[(REG_BG - VGC_BASE) as usize] & 0x0F;
        (bg << 4) | fg
    }

    fn queue_byte(&mut self, value: u8) {
        let next = self.input_head.wrapping_add(1);
        if next == self.input_tail {
            return;
        }
        self.input[self.input_head as usize] = value;
        self.input_head = next;
    }

    fn dequeue_byte(&mut self) -> Option<u8> {
        if self.input_tail == self.input_head {
            return None;
        }
        let value = self.input[self.input_tail as usize];
        self.input_tail = self.input_tail.wrapping_add(1);
        Some(value)
    }

    fn read_word(&mut self, addr: u16) -> u16 {
        let lo = self.read(addr) as u16;
        let hi = self.read(addr.wrapping_add(1)) as u16;
        lo | (hi << 8)
    }

    fn imm(&self) -> u8 {
        self.ram[self.pc.wrapping_add(1) as usize]
    }
    fn abs(&self) -> u16 {
        let lo = self.ram[self.pc.wrapping_add(1) as usize] as u16;
        let hi = self.ram[self.pc.wrapping_add(2) as usize] as u16;
        lo | (hi << 8)
    }
    fn zp(&self) -> u16 {
        self.imm() as u16
    }
    fn zpx(&self) -> u16 {
        self.imm().wrapping_add(self.x) as u16
    }
    fn zpy(&self) -> u16 {
        self.imm().wrapping_add(self.y) as u16
    }
    fn zpi(&mut self) -> u16 {
        let addr = self.imm() as u16;
        self.read_word(addr)
    }
    fn indx(&mut self) -> u16 {
        let addr = self.imm().wrapping_add(self.x) as u16;
        self.read_word(addr)
    }

    fn absx(&self, page_cycle: bool) -> (u16, u8) {
        let base = self.abs();
        let addr = base.wrapping_add(self.x as u16);
        (
            addr,
            if page_cycle && page_crossed(base, addr) {
                1
            } else {
                0
            },
        )
    }

    fn absy(&self, page_cycle: bool) -> (u16, u8) {
        let base = self.abs();
        let addr = base.wrapping_add(self.y as u16);
        (
            addr,
            if page_cycle && page_crossed(base, addr) {
                1
            } else {
                0
            },
        )
    }

    fn indy(&mut self, page_cycle: bool) -> (u16, u8) {
        let zp = self.imm() as u16;
        let base = self.read_word(zp);
        let addr = base.wrapping_add(self.y as u16);
        (
            addr,
            if page_cycle && page_crossed(base, addr) {
                1
            } else {
                0
            },
        )
    }

    fn push(&mut self, value: u8) {
        let addr = 0x0100 | self.sp as u16;
        self.ram[addr as usize] = value;
        self.sp = self.sp.wrapping_sub(1);
    }

    fn pop(&mut self) -> u8 {
        self.sp = self.sp.wrapping_add(1);
        self.ram[(0x0100 | self.sp as u16) as usize]
    }

    fn push_word(&mut self, value: u16) {
        self.push((value >> 8) as u8);
        self.push(value as u8);
    }

    fn pop_word(&mut self) -> u16 {
        let lo = self.pop() as u16;
        let hi = self.pop() as u16;
        lo | (hi << 8)
    }

    fn irq(&mut self, vector: u16, brk: bool) {
        self.push((self.pc >> 8) as u8);
        self.push(self.pc as u8);
        self.push((self.p & !B) | U | if brk { B } else { 0 });
        self.set_flag(I, true);
        if self.cmos {
            self.set_flag(D, false);
        }
        self.pc = self.read_word(vector);
    }

    fn branch(&mut self, take: bool) -> u8 {
        let next = self.pc.wrapping_add(2);
        if !take {
            self.pc = next;
            return 2;
        }
        let target = next.wrapping_add((self.imm() as i8) as i16 as u16);
        self.pc = target;
        3 + if page_crossed(next, target) { 1 } else { 0 }
    }

    fn bit_branch(&mut self, mask: u8, branch_on_set: bool) -> u8 {
        let value = self.read(self.zp());
        let next = self.pc.wrapping_add(3);
        let bit_is_set = value & mask != 0;
        if bit_is_set == branch_on_set {
            let offset = self.ram[self.pc.wrapping_add(2) as usize] as i8;
            self.pc = next.wrapping_add(offset as i16 as u16);
        } else {
            self.pc = next;
        }
        5
    }

    fn set_zn(&mut self, value: u8) {
        self.set_flag(Z, value == 0);
        self.set_flag(N, value & 0x80 != 0);
    }

    fn flag(&self, flag: u8) -> bool {
        self.p & flag != 0
    }

    fn set_flag(&mut self, flag: u8, set: bool) {
        if set {
            self.p |= flag;
        } else {
            self.p &= !flag;
        }
        self.p |= U;
    }

    fn compare(&mut self, left: u8, right: u8) {
        let result = left.wrapping_sub(right);
        self.set_flag(C, left >= right);
        self.set_flag(Z, left == right);
        self.set_flag(N, result & 0x80 != 0);
    }

    fn inc(&mut self, value: u8) -> u8 {
        let result = value.wrapping_add(1);
        self.set_zn(result);
        result
    }

    fn dec(&mut self, value: u8) -> u8 {
        let result = value.wrapping_sub(1);
        self.set_zn(result);
        result
    }

    fn adc(&mut self, value: u8) {
        if self.flag(D) {
            let mut result =
                bcd_to_int(self.a) + bcd_to_int(value) + if self.flag(C) { 1 } else { 0 };
            self.set_flag(C, result > 99);
            if result > 99 {
                result -= 100;
            }
            self.a = int_to_bcd(result);
            self.set_flag(Z, result == 0);
            self.set_flag(N, self.a & 0x80 != 0);
            return;
        }
        let carry = if self.flag(C) { 1 } else { 0 };
        let sum = self.a as u16 + value as u16 + carry;
        let result = sum as u8;
        self.set_flag(C, sum > 0xFF);
        self.set_flag(V, (!(self.a ^ value) & (self.a ^ result) & 0x80) != 0);
        self.a = result;
        self.set_zn(self.a);
    }

    fn sbc(&mut self, value: u8) {
        if self.flag(D) {
            let mut result =
                bcd_to_int(self.a) - bcd_to_int(value) - if self.flag(C) { 0 } else { 1 };
            self.set_flag(C, result >= 0);
            if result < 0 {
                result += 100;
            }
            self.a = int_to_bcd(result);
            self.set_flag(Z, result == 0);
            self.set_flag(N, self.a & 0x80 != 0);
            return;
        }
        self.adc(!value);
    }

    fn asl(&mut self, value: u8) -> u8 {
        self.set_flag(C, value & 0x80 != 0);
        let result = value << 1;
        self.set_zn(result);
        result
    }

    fn lsr(&mut self, value: u8) -> u8 {
        self.set_flag(C, value & 1 != 0);
        let result = value >> 1;
        self.set_zn(result);
        result
    }

    fn rol(&mut self, value: u8) -> u8 {
        let old_carry = self.flag(C);
        self.set_flag(C, value & 0x80 != 0);
        let result = (value << 1) | if old_carry { 1 } else { 0 };
        self.set_zn(result);
        result
    }

    fn ror(&mut self, value: u8) -> u8 {
        let old_carry = self.flag(C);
        self.set_flag(C, value & 1 != 0);
        let result = (value >> 1) | if old_carry { 0x80 } else { 0 };
        self.set_zn(result);
        result
    }

    fn bit(&mut self, value: u8, update_nv: bool) {
        self.set_flag(Z, self.a & value == 0);
        if update_nv {
            self.set_flag(N, value & 0x80 != 0);
            self.set_flag(V, value & 0x40 != 0);
        }
    }

    fn dirty(&mut self) {
        self.dirty_version = self.dirty_version.wrapping_add(1);
    }
}

fn bcd_to_int(value: u8) -> i16 {
    ((value >> 4) as i16) * 10 + (value & 0x0F) as i16
}

fn int_to_bcd(value: i16) -> u8 {
    (((value / 10) as u8) << 4) | ((value % 10) as u8)
}

fn next_rng(state: u32) -> u32 {
    let mut y = if state == 0 { 0xDEADBEEF } else { state };
    y ^= y << 13;
    y ^= y >> 17;
    y ^= y << 5;
    if y == 0 {
        0xDEADBEEF
    } else {
        y
    }
}

fn rect_extent(width: usize, height: usize, stride: usize) -> usize {
    if height == 0 {
        0
    } else {
        (height - 1).saturating_mul(stride).saturating_add(width)
    }
}

fn rect_linear_ranges_overlap(
    a_base: usize,
    a_width: usize,
    a_height: usize,
    a_stride: usize,
    b_base: usize,
    b_width: usize,
    b_height: usize,
    b_stride: usize,
) -> bool {
    let a_end = a_base.saturating_add(rect_extent(a_width, a_height, a_stride));
    let b_end = b_base.saturating_add(rect_extent(b_width, b_height, b_stride));
    a_base < b_end && b_base < a_end
}

fn rotated_source_offset(size: usize, row: usize, col: usize, angle: u8) -> Option<(usize, usize)> {
    let sin = sin1p7(angle) as i8 as i64;
    let cos = sin1p7(angle.wrapping_add(64)) as i8 as i64;
    let center_fp = ((size as i64) - 1) << 7;
    let dx = ((col as i64) << 8) - center_fp;
    let dy = ((row as i64) << 8) - center_fp;
    let src_x_fp = center_fp + (((dx * cos) + (dy * sin)) >> 7);
    let src_y_fp = center_fp + (((dy * cos) - (dx * sin)) >> 7);
    let src_x = (src_x_fp + 128) >> 8;
    let src_y = (src_y_fp + 128) >> 8;

    if src_x < 0 || src_y < 0 || src_x >= size as i64 || src_y >= size as i64 {
        None
    } else {
        Some((src_y as usize, src_x as usize))
    }
}

fn saturate_i16(value: i64) -> (u16, u8) {
    if value > i16::MAX as i64 {
        (0x7FFF, MATH_STATUS_OVERFLOW)
    } else if value < i16::MIN as i64 {
        (0x8000, MATH_STATUS_OVERFLOW)
    } else {
        (value as i16 as u16, MATH_STATUS_OK)
    }
}

fn sin1p7(angle: u8) -> u8 {
    let fraction = (angle & 0x3F) as usize;
    let (index, negate) = match angle >> 6 {
        0 => (fraction, false),
        1 => (64 - fraction, false),
        2 => (fraction, true),
        _ => (64 - fraction, true),
    };
    let magnitude = SIN_QUARTER_MAGNITUDES[index] as i16;
    let value = if negate {
        -magnitude
    } else if magnitude > i8::MAX as i16 {
        i8::MAX as i16
    } else {
        magnitude
    };
    value as i8 as u8
}

fn abs_i16_to_i32(value: i16) -> i32 {
    if value == i16::MIN {
        0x8000
    } else if value < 0 {
        -(value as i32)
    } else {
        value as i32
    }
}

fn dist_approx(dx: i16, dy: i16) -> u16 {
    let ax = abs_i16_to_i32(dx);
    let ay = abs_i16_to_i32(dy);
    let max = if ax > ay { ax } else { ay };
    let min = if ax < ay { ax } else { ay };
    let approx = max + (min >> 1) - (min >> 3);
    if approx > u16::MAX as i32 {
        u16::MAX
    } else {
        approx as u16
    }
}

fn atan2_angle(dy: i16, dx: i16) -> u8 {
    if dx == 0 && dy == 0 {
        return 0;
    }
    if dy == 0 {
        return if dx < 0 { 128 } else { 0 };
    }
    if dx == 0 {
        return if dy < 0 { 192 } else { 64 };
    }

    let steps: [i32; 8] = [32, 19, 10, 5, 3, 1, 1, 0];
    let mut x = dx as i32;
    let mut y = dy as i32;
    let mut z = 0i32;
    if x < 0 {
        x = -x;
        y = -y;
        z = 128;
    }
    let mut i = 0usize;
    while i < steps.len() {
        if y > 0 {
            let nx = x + (y >> i);
            let ny = y - (x >> i);
            x = nx;
            y = ny;
            z += steps[i];
        } else if y < 0 {
            let nx = x - (y >> i);
            let ny = y + (x >> i);
            x = nx;
            y = ny;
            z -= steps[i];
        }
        i += 1;
    }
    z as u8
}

fn page_crossed(a: u16, b: u16) -> bool {
    (a ^ b) & 0xFF00 != 0
}

fn bit_mask(opcode: u8) -> u8 {
    1 << ((opcode >> 4) & 0x07)
}

fn min_usize(a: usize, b: usize) -> usize {
    if a < b {
        a
    } else {
        b
    }
}

fn is_host_fio_command(command: u8) -> bool {
    matches!(
        command,
        FIO_CMD_SAVE
            | FIO_CMD_LOAD
            | FIO_CMD_DIR_OPEN
            | FIO_CMD_DIR_READ
            | FIO_CMD_DELETE
            | FIO_CMD_GSAVE
            | FIO_CMD_GLOAD
            | FIO_CMD_XLOAD
            | FIO_CMD_XSAVE
            | FIO_CMD_CD
            | FIO_CMD_MKDIR
            | FIO_CMD_RMDIR
            | FIO_CMD_MOUNT
            | FIO_CMD_UNMOUNT
            | FIO_CMD_PWD
            | FIO_CMD_LOAD_RUNTIME
            | FIO_CMD_XPAGE
            | FIO_CMD_NVGLOAD
            | FIO_CMD_FOPEN..=FIO_CMD_DEVSTATUS
    )
}

fn min_u8(a: u8, b: u8) -> u8 {
    if a < b {
        a
    } else {
        b
    }
}

fn min_i32(a: i32, b: i32) -> i32 {
    if a < b {
        a
    } else {
        b
    }
}

fn max_i32(a: i32, b: i32) -> i32 {
    if a > b {
        a
    } else {
        b
    }
}

fn abs_i32(value: i32) -> i32 {
    if value < 0 {
        -value
    } else {
        value
    }
}

fn resolve_copper_register(register_specifier: u16) -> Option<u8> {
    if register_specifier == COPPER_REG_IRQ as u16 {
        return Some(COPPER_REG_IRQ);
    }

    if register_specifier < 16 {
        let register_index = register_specifier as u8;
        return if is_copper_writable_register(register_index) {
            Some(register_index)
        } else {
            None
        };
    }

    if (VGC_BASE..VGC_BASE + 16).contains(&register_specifier) {
        let register_index = (register_specifier - VGC_BASE) as u8;
        return if is_copper_writable_register(register_index) {
            Some(register_index)
        } else {
            None
        };
    }

    if register_specifier == REG_SCROLL_CTL || register_specifier == REG_SCROLL_CTL - VGC_BASE {
        return Some((REG_SCROLL_CTL - VGC_BASE) as u8);
    }

    if (SPRITE_REG_BASE..=SPRITE_REG_END).contains(&register_specifier) {
        let register_index = (register_specifier - VGC_BASE) as u8;
        return if is_copper_writable_register(register_index) {
            Some(register_index)
        } else {
            None
        };
    }

    None
}

fn is_copper_writable_register(register_index: u8) -> bool {
    register_index == (REG_MODE - VGC_BASE) as u8
        || register_index == (REG_BG - VGC_BASE) as u8
        || register_index == (REG_FG - VGC_BASE) as u8
        || register_index == (REG_BORDER - VGC_BASE) as u8
        || register_index == (REG_SCROLL_X - VGC_BASE) as u8
        || register_index == (REG_SCROLL_Y - VGC_BASE) as u8
        || register_index == (REG_SCROLL_CTL - VGC_BASE) as u8
        || is_copper_sprite_register(register_index)
}

fn is_copper_sprite_register(register_index: u8) -> bool {
    let offset = register_index as i32 - (SPRITE_REG_BASE - VGC_BASE) as i32;
    offset >= 0 && offset < (MAX_SPRITES * SPRITE_REG_STRIDE) as i32
}

fn out_code(x: i32, y: i32) -> i32 {
    let mut code = 0;
    if x < 0 {
        code |= 1;
    } else if x >= GFX_WIDTH as i32 {
        code |= 2;
    }
    if y < 0 {
        code |= 8;
    } else if y >= GFX_HEIGHT as i32 {
        code |= 4;
    }
    code
}

fn clip_line(x0: &mut i32, y0: &mut i32, x1: &mut i32, y1: &mut i32) -> bool {
    let xmax = GFX_WIDTH as i32 - 1;
    let ymax = GFX_HEIGHT as i32 - 1;
    let mut c0 = out_code(*x0, *y0);
    let mut c1 = out_code(*x1, *y1);

    loop {
        if c0 | c1 == 0 {
            return true;
        }
        if c0 & c1 != 0 {
            return false;
        }

        let out = if c0 != 0 { c0 } else { c1 };
        let (x, y) = if out & 8 != 0 {
            (*x0 + (*x1 - *x0) * (0 - *y0) / (*y1 - *y0), 0)
        } else if out & 4 != 0 {
            (*x0 + (*x1 - *x0) * (ymax - *y0) / (*y1 - *y0), ymax)
        } else if out & 2 != 0 {
            (xmax, *y0 + (*y1 - *y0) * (xmax - *x0) / (*x1 - *x0))
        } else {
            (0, *y0 + (*y1 - *y0) * (0 - *x0) / (*x1 - *x0))
        };

        if out == c0 {
            *x0 = x;
            *y0 = y;
            c0 = out_code(*x0, *y0);
        } else {
            *x1 = x;
            *y1 = y;
            c1 = out_code(*x1, *y1);
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::sync::{Mutex, MutexGuard};

    const MAX_STEPS: usize = 120_000_000;

    static TEST_VM: Mutex<Vm> = Mutex::new(Vm::new());

    fn load_fixture(program: &[u8], load: usize) -> MutexGuard<'static, Vm> {
        let mut vm = TEST_VM
            .lock()
            .unwrap_or_else(|poisoned| poisoned.into_inner());
        vm.reset();
        vm.flat_bus = true;
        vm.ram[load..load + program.len()].copy_from_slice(program);
        vm.pc = 0x0400;
        vm.p = I | U;
        vm
    }

    fn blank_vm() -> MutexGuard<'static, Vm> {
        let mut vm = TEST_VM
            .lock()
            .unwrap_or_else(|poisoned| poisoned.into_inner());
        vm.reset();
        vm
    }

    fn run_until_self_loop(vm: &mut Vm, pass_pc: u16, interrupts: bool) {
        let mut trace = [0u16; 32];
        let mut trace_index = 0usize;
        for _ in 0..MAX_STEPS {
            let prev_pc = vm.pc;
            trace[trace_index % trace.len()] = prev_pc;
            trace_index = trace_index.wrapping_add(1);
            vm.step();
            assert!(
                !vm.trapped,
                "trapped at ${:04X} opcode ${:02X}",
                vm.trap_pc, vm.trap_opcode
            );

            if interrupts {
                match prev_pc {
                    0x0434 | 0x0464 | 0x04A3 | 0x04DE => vm.irq_waiting = true,
                    0x05C8 | 0x05F8 | 0x0637 | 0x0672 => vm.nmi_waiting = true,
                    0x06A0 | 0x06DB => {
                        vm.irq_waiting = true;
                        vm.nmi_waiting = true;
                    }
                    _ => {}
                }
            }

            if prev_pc == vm.pc {
                assert_eq!(
                    pass_pc,
                    vm.pc,
                    "test program failed at ${:04X} A=${:02X} X=${:02X} Y=${:02X} SP=${:02X} P=${:02X} I_SRC=${:02X} IRQ_COUNT=${:02X} NMI_COUNT=${:02X} BRK_COUNT=${:02X} trace={:04X?}",
                    vm.pc,
                    vm.a,
                    vm.x,
                    vm.y,
                    vm.sp,
                    vm.p,
                    vm.ram[0x0203],
                    vm.ram[0x0201],
                    vm.ram[0x0200],
                    vm.ram[0x0202],
                    trace
                );
                return;
            }
        }
        panic!("test timed out at ${:04X}", vm.pc);
    }

    #[test]
    fn all_suite_a_passes() {
        let mut vm = load_fixture(
            include_bytes!("../../e6502UnitTests/Resources/AllSuiteA.bin"),
            0x4000,
        );
        run_until_self_loop(&mut vm, 0x45C0, false);
        assert_eq!(0xFF, vm.ram[0x0210], "AllSuiteA status byte failed");
    }

    #[test]
    fn klaus_functional_passes() {
        let mut vm = load_fixture(
            include_bytes!("../../e6502UnitTests/Resources/6502_functional_test.bin"),
            0,
        );
        run_until_self_loop(&mut vm, 0x3399, false);
    }

    #[test]
    fn cmos_extended_opcodes_pass() {
        let mut vm = load_fixture(
            include_bytes!("../../e6502UnitTests/Resources/65C02_extended_opcodes_test.bin"),
            0,
        );
        run_until_self_loop(&mut vm, 0x24A8, false);
    }

    #[test]
    fn klaus_interrupts_pass() {
        let mut vm = load_fixture(
            include_bytes!("../../e6502UnitTests/Resources/6502_interrupt_test.bin"),
            0x0400,
        );
        vm.cmos = false;
        run_until_self_loop(&mut vm, 0x06EC, true);
    }

    #[test]
    fn sprite_registers_and_shape_ram_are_mapped() {
        let mut vm = blank_vm();
        vm.write_io(SPRITE_REG_BASE, 0x34);
        vm.write_io(SPRITE_REG_BASE + 1, 0x01);
        vm.write_io(SPRITE_REG_BASE + 2, 0x22);
        vm.write_io(SPRITE_REG_BASE + 5, SPR_FLAG_ENABLE);
        assert_eq!(0x34, vm.read_io(SPRITE_REG_BASE));
        assert_eq!(0x01, vm.read_io(SPRITE_REG_BASE + 1));
        assert_eq!(1, vm.vgc_regs[(REG_SPRITE_COUNT - VGC_BASE) as usize]);

        vm.write_io(REG_CMD + 1, 0);
        vm.write_io(REG_CMD + 2, 2);
        vm.write_io(REG_CMD + 3, 3);
        vm.write_io(REG_CMD + 4, 0x0A);
        vm.write_io(REG_CMD, CMD_SPR_DEF);
        let shape_index = 3 * 8 + 1;
        assert_eq!(0xA0, vm.sprite_shapes[shape_index] & 0xF0);

        vm.write_io(VRAM_PLANE, MEM_SPACE_SPRITE);
        vm.write_io(VRAM_ADDR_L, 0);
        vm.write_io(VRAM_ADDR_H, 0);
        vm.write_io(VRAM_DATA, 0x5A);
        assert_eq!(0x5A, vm.sprite_shapes[0]);
    }

    #[test]
    fn copper_commands_build_active_program() {
        let mut vm = blank_vm();
        vm.write_io(REG_CMD + 1, 40);
        vm.write_io(REG_CMD + 2, 0);
        vm.write_io(REG_CMD + 3, 10);
        vm.write_io(REG_CMD + 4, (REG_MODE - VGC_BASE) as u8);
        vm.write_io(REG_CMD + 5, 0);
        vm.write_io(REG_CMD + 6, 3);
        vm.write_io(REG_CMD, CMD_COPPER_ADD);
        vm.write_io(REG_CMD, CMD_COPPER_ENABLE);

        assert!(vm.copper_enabled);
        assert_eq!(1, vm.copper_counts[0]);
        assert_eq!(10 * GFX_WIDTH as u16 + 40, vm.copper_events[0][0].position);
        assert_eq!(
            (REG_MODE - VGC_BASE) as u8,
            vm.copper_events[0][0].register_index
        );
        assert_eq!(3, vm.copper_events[0][0].value);

        vm.write_io(REG_CMD, CMD_COPPER_CLEAR);
        assert_eq!(0, vm.copper_counts[0]);
    }

    #[test]
    fn copper_use_switches_active_list_at_vblank() {
        let mut vm = blank_vm();
        vm.write_io(REG_CMD + 1, 1);
        vm.write_io(REG_CMD, CMD_COPPER_LIST);

        vm.write_io(REG_CMD + 1, 0);
        vm.write_io(REG_CMD + 2, 0);
        vm.write_io(REG_CMD + 3, 25);
        vm.write_io(REG_CMD + 4, (REG_BG - VGC_BASE) as u8);
        vm.write_io(REG_CMD + 5, 0);
        vm.write_io(REG_CMD + 6, 5);
        vm.write_io(REG_CMD, CMD_COPPER_ADD);

        vm.write_io(REG_CMD + 1, 1);
        vm.write_io(REG_CMD, CMD_COPPER_USE);
        assert_eq!(0, vm.copper_active_list);
        assert_eq!(0, vm.copper_counts[vm.copper_active_list]);

        vm.vblank();
        assert_eq!(1, vm.copper_active_list);
        assert_eq!(1, vm.copper_counts[vm.copper_active_list]);
        assert_eq!(5, vm.copper_events[vm.copper_active_list][0].value);
    }

    #[test]
    fn copper_accepts_irq_events() {
        let mut vm = blank_vm();
        vm.write_io(REG_CMD + 1, 0);
        vm.write_io(REG_CMD + 2, 0);
        vm.write_io(REG_CMD + 3, 0);
        vm.write_io(REG_CMD + 4, COPPER_REG_IRQ);
        vm.write_io(REG_CMD + 5, 0);
        vm.write_io(REG_CMD + 6, IRQ_COPPER0);
        vm.write_io(REG_CMD, CMD_COPPER_ADD);

        assert_eq!(1, vm.copper_counts[0]);
        assert_eq!(COPPER_REG_IRQ, vm.copper_events[0][0].register_index);
        assert_eq!(IRQ_COPPER0, vm.copper_events[0][0].value);
    }

    #[test]
    fn dma_copies_cpu_ram_to_gfx_memory() {
        let mut vm = blank_vm();
        vm.ram[0x2000] = 1;
        vm.ram[0x2001] = 2;
        vm.ram[0x2002] = 3;
        vm.ram[0x2003] = 4;
        vm.write_io(DMA_SRC_SPACE, DMA_SPACE_CPU_RAM);
        vm.write_io(DMA_DST_SPACE, DMA_SPACE_VGC_GFX);
        vm.write_io(DMA_SRC_L, 0x00);
        vm.write_io(DMA_SRC_L + 1, 0x20);
        vm.write_io(DMA_SRC_L + 2, 0x00);
        vm.write_io(DMA_DST_L, 0x10);
        vm.write_io(DMA_DST_L + 1, 0x00);
        vm.write_io(DMA_DST_L + 2, 0x00);
        vm.write_io(DMA_LEN_L, 4);
        vm.write_io(DMA_LEN_L + 1, 0);
        vm.write_io(DMA_LEN_L + 2, 0);
        vm.write_io(DMA_CMD, DMA_CMD_START);
        assert_eq!(DMA_STATUS_OK, vm.read_io(DMA_STATUS));
        assert_eq!([1, 2, 3, 4], vm.gfx[0x10..0x14]);
    }

    #[test]
    fn blitter_fills_rectangles_in_gfx_memory() {
        let mut vm = blank_vm();
        vm.write_io(BLT_DST_SPACE, DMA_SPACE_VGC_GFX);
        vm.write_io(BLT_DST_L, 0);
        vm.write_io(BLT_DST_L + 1, 0);
        vm.write_io(BLT_DST_L + 2, 0);
        vm.write_io(BLT_WIDTH_L, 3);
        vm.write_io(BLT_WIDTH_L + 1, 0);
        vm.write_io(BLT_HEIGHT_L, 2);
        vm.write_io(BLT_HEIGHT_L + 1, 0);
        vm.write_io(BLT_DST_STRIDE_L, GFX_WIDTH as u8);
        vm.write_io(BLT_DST_STRIDE_L + 1, (GFX_WIDTH >> 8) as u8);
        vm.write_io(BLT_MODE, BLT_MODE_FILL);
        vm.write_io(BLT_FILL_VALUE, 7);
        vm.write_io(BLT_CMD, BLT_CMD_START);
        assert_eq!(BLT_STATUS_OK, vm.read_io(BLT_STATUS));
        assert_eq!(7, vm.gfx[0]);
        assert_eq!(7, vm.gfx[2]);
        assert_eq!(7, vm.gfx[GFX_WIDTH]);
        assert_eq!(7, vm.gfx[GFX_WIDTH + 2]);
    }

    #[test]
    fn blitter_rotates_square_into_destination_buffer() {
        let mut vm = blank_vm();
        let src = 0x3600usize;
        let dst = 0x3700usize;
        for row in 0..3 {
            for col in 0..3 {
                vm.ram[src + row * 3 + col] = (row * 3 + col + 1) as u8;
            }
        }

        vm.write_io(BLT_SRC_SPACE, DMA_SPACE_CPU_RAM);
        vm.write_io(BLT_DST_SPACE, DMA_SPACE_CPU_RAM);
        vm.write_io(BLT_SRC_L, src as u8);
        vm.write_io(BLT_SRC_L + 1, (src >> 8) as u8);
        vm.write_io(BLT_SRC_L + 2, 0);
        vm.write_io(BLT_DST_L, dst as u8);
        vm.write_io(BLT_DST_L + 1, (dst >> 8) as u8);
        vm.write_io(BLT_DST_L + 2, 0);
        vm.write_io(BLT_WIDTH_L, 3);
        vm.write_io(BLT_WIDTH_L + 1, 0);
        vm.write_io(BLT_HEIGHT_L, 3);
        vm.write_io(BLT_HEIGHT_L + 1, 0);
        vm.write_io(BLT_SRC_STRIDE_L, 3);
        vm.write_io(BLT_SRC_STRIDE_L + 1, 0);
        vm.write_io(BLT_DST_STRIDE_L, 3);
        vm.write_io(BLT_DST_STRIDE_L + 1, 0);
        vm.write_io(BLT_MODE, BLT_MODE_ROTATE);
        vm.write_io(BLT_ROTATE_ANGLE, 64);
        vm.write_io(BLT_CMD, BLT_CMD_START);

        assert_eq!(BLT_STATUS_OK, vm.read_io(BLT_STATUS));
        assert_eq!(9, vm.read_io(BLT_COUNT_L));
        assert_eq!([7, 4, 1, 8, 5, 2, 9, 6, 3], vm.ram[dst..dst + 9]);
    }

    #[test]
    fn timer_sets_and_clears_irq_status() {
        let mut vm = blank_vm();
        vm.write_io(TIMER_DIV_L, 1);
        vm.write_io(TIMER_DIV_H, 0);
        vm.write_io(TIMER_CTRL, 1);
        vm.advance_hardware(TIMER_TICK_QUANTUM_CYCLES as u32);
        assert!(vm.irq_waiting);
        assert_eq!(1, vm.read_io(TIMER_STATUS));
        assert_eq!(0, vm.read_io(TIMER_STATUS));
    }

    #[test]
    fn math_mul16_writes_32_bit_result() {
        let mut vm = blank_vm();
        vm.write_io(0xBB20, 0x34);
        vm.write_io(0xBB21, 0x12);
        vm.write_io(0xBB22, 0x02);
        vm.write_io(0xBB23, 0x00);
        let result = vm.read_io(MATH_RES0) as u32
            | ((vm.read_io(MATH_RES0 + 1) as u32) << 8)
            | ((vm.read_io(MATH_RES0 + 2) as u32) << 16)
            | ((vm.read_io(MATH_RES0 + 3) as u32) << 24);
        assert_eq!(0x2468, result);
        assert_eq!(MATH_STATUS_OK, vm.read_io(MATH_STATUS));
    }
}
