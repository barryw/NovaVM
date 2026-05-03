#!/usr/bin/env python3
"""Static contract tests for NovaHost WiFi management.

These tests intentionally avoid Arduino or RF dependencies. They verify the
public management contract that has to remain stable across refactors:

- USB serial bootstrap commands exist.
- REST endpoints are routed.
- GET /wifi redacts the password and exposes only passwordSet.
- Host-status bits drive the FPGA WiFi LED as:
  off = unconfigured, flashing = configured but disconnected, solid = connected.
- Runtime-replacing autoboots can load a 16K ROM from a floppy image before HD.
"""

from pathlib import Path
import re
import sys


ROOT = Path(__file__).resolve().parents[1]
REPO = ROOT.parents[1]


def read(rel: str) -> str:
    return (REPO / rel).read_text(encoding="utf-8")


def check(name: str, ok: bool) -> None:
    print(f"  {'PASS' if ok else 'FAIL'}  {name}")
    if not ok:
        raise AssertionError(name)


def test_serial_commands() -> None:
    src = read("e6502.ESP32/novahost/novahost.ino")
    for command in [
        'wifi status',
        'wifi scan',
        'wifi set ssid',
        'wifi set password',
        'wifi set dhcp on',
        'wifi set static',
        'wifi connect',
        'wifi disconnect',
        'wifi reconnect',
        'wifi forget',
    ]:
        check(f"serial command advertised: {command}", command in src)

    check("serial console is polled from loop()", "pollSerialConsole();" in src)
    check("serial parser handles quoted args", "bool inQuote = false" in src)


def test_rest_routes() -> None:
    src = read("e6502.ESP32/novahost/sd_http_server.cpp")
    header = read("e6502.ESP32/novahost/sd_http_server.h")

    for route in [
        'GET    /wifi',
        'GET    /wifi/scan',
        'PUT    /wifi',
        'POST   /wifi/connect|disconnect|reconnect|forget',
    ]:
        check(f"REST route documented: {route}", route in header)

    checks = {
        "GET /wifi dispatch": 'strcmp(method, "GET") == 0 && strcmp(url, "/wifi") == 0',
        "GET /wifi/scan dispatch": 'strcmp(method, "GET") == 0 && strcmp(url, "/wifi/scan") == 0',
        "PUT /wifi dispatch": 'strcmp(method, "PUT") == 0 && strcmp(url, "/wifi") == 0',
        "POST /wifi action dispatch": 'strcmp(method, "POST") == 0 && strncmp(url, "/wifi/", 6) == 0',
        "GET /health exposes host-status flags": '\\"hostStatusHex\\"',
        "connect action": 'strcmp(action, "connect") == 0',
        "disconnect action": 'strcmp(action, "disconnect") == 0',
        "reconnect action": 'strcmp(action, "reconnect") == 0',
        "forget action": 'strcmp(action, "forget") == 0',
    }
    for name, needle in checks.items():
        check(name, needle in src)


def test_password_redaction() -> None:
    src = read("e6502.ESP32/novahost/sd_http_server.cpp")
    status_body = src.split("void SdHttpServer::handle_wifi_status", 1)[1]
    status_body = status_body.split("void SdHttpServer::handle_wifi_scan", 1)[0]

    check("GET /wifi exposes passwordSet", '\\"passwordSet\\"' in status_body)
    check("GET /wifi does not emit password field", '\\"password\\"' not in status_body)
    check("PUT /wifi accepts password updates", 'doc["password"]' in src)


def test_host_status_led_contract() -> None:
    novahost = read("e6502.ESP32/novahost/novahost.ino")
    fpga = read("e6502.FPGA/fpga/fpga_top.sv")

    connected = re.search(r"HOST_STATUS_WIFI_CONNECTED\s*=\s*0x01", novahost)
    configured = re.search(r"HOST_STATUS_WIFI_CONFIGURED\s*=\s*0x40", novahost)
    check("WiFi connected bit is host-status bit 0", connected is not None)
    check("WiFi configured bit is host-status bit 6", configured is not None)
    check(
        "host-status masks clear stale bits before setting new bits",
        "uint8_t next = (current & (uint8_t)~clear_mask) | set_mask;" in novahost,
    )

    check(
        "configured bit is set from novaWifi.configured()",
        "if (novaWifi.configured())" in novahost
        and "set_mask |= HOST_STATUS_WIFI_CONFIGURED" in novahost,
    )
    check(
        "connected bit is set from novaWifi.connected()",
        "if (novaWifi.connected())" in novahost
        and "set_mask |= HOST_STATUS_WIFI_CONNECTED" in novahost,
    )
    check(
        "host-status latch is periodically force-refreshed after FPGA reload",
        "publishHostStatusToFpga(true);" in novahost
        and "lastHostStatusRefresh" in novahost,
    )
    check(
        "FPGA LED is solid when connected and flashes when only configured",
        "wire status_flash = heartbeat[23];" in fpga
        and "wire wifi_led = brg_host_status[0] ? 1'b1 :" in fpga
        and "(brg_host_status[6] ? status_flash : 1'b0);" in fpga,
    )
    check(
        "FIO and degraded boot LEDs flash at the heartbeat rate",
        "wire fio_led = brg_host_status[2] ? status_flash : brg_host_status[1];" in fpga
        and "wire boot_led = brg_host_status[5] ? status_flash : brg_host_status[4];" in fpga,
    )
    check(
        "FPGA LED panel drives active-high ULX3S LEDs directly",
        "assign leds = led_operator_mode ? operator_leds : user_leds;" in fpga,
    )
    check(
        "operator LED page button is initialized idle-low",
        "reg [1:0]  btn6_sync = 2'b00;" in fpga
        and "reg        btn6_stable = 1'b0;" in fpga,
    )
    check(
        "operator LED page toggles on RIGHT button press",
        "if (!btn6_stable && btn6_sync[1])" in fpga,
    )


def test_fio_clear_error_contract() -> None:
    basic = read("ehbasic/basic.asm")
    fio = read("ehbasic/lib/fio.s")
    nova_inc = read("ehbasic/lib/nova.inc")
    dispatcher_h = read("e6502.ESP32/novahost/fio_dispatcher.h")
    dispatcher_cpp = read("e6502.ESP32/novahost/fio_dispatcher.cpp")
    constants = read("e6502.Avalonia/Hardware/VgcConstants.cs")
    controller = read("e6502.Avalonia/Hardware/FileIoController.cs")
    unit_tests = read("e6502UnitTests/FileIoControllerTests.cs")
    integration = read("tests/integration/fio.6502")

    checks = {
        "BASIC FIO command ID is reserved": "FIO_CMD_CLEARERR  = $27" in nova_inc,
        "BASIC helper exports fio_clear_error": ".export fio_clear_error" in fio,
        "BASIC helper issues clear command": "LDA   #FIO_CMD_CLEARERR" in fio,
        "BASIC FIO exec clears stale status before command": "fio_exec:" in fio
        and "STZ   FIO_STATUS" in fio
        and "STZ   FIO_ERRCODE" in fio
        and "@wait:" in fio,
        "BASIC helper clears stale local FIO status first": "STA   FIO_ERRCODE" in fio and "STA   FIO_STATUS" in fio,
        "BASIC command token exists": "XTK_FIOCLR         = $71" in basic and '"FIOCLR",0' in basic,
        "BASIC command dispatches fio_clear_error": "LAB_FIOCLR" in basic and "JSR   fio_clear_error" in basic,
        "ESP dispatcher command ID matches": "CMD_CLEARERR = 0x27" in dispatcher_h,
        "ESP dispatcher handles clear command": "case CMD_CLEARERR: handle_clear_error(); break;" in dispatcher_cpp,
        "ESP clear command responds OK": "void FioDispatcher::handle_clear_error()" in dispatcher_cpp and "respond_ok();" in dispatcher_cpp,
        "emulator command ID matches": "FioCmdClearErr   = 0x27" in constants,
        "emulator handles clear command": "case VgcConstants.FioCmdClearErr:" in controller,
        "emulator regression test covers clear command": "ClearErr_ClearsLatchedFileIoError" in unit_tests,
        "hardware integration suite covers FIOCLR": "FIOCLR" in integration and "$B9A1" in integration,
    }
    for name, ok in checks.items():
        check(name, ok)


def test_fio_sd_dispatch_contract() -> None:
    header = read("e6502.ESP32/novahost/fio_dispatcher.h")
    dispatcher = read("e6502.ESP32/novahost/fio_dispatcher.cpp")
    device_manager = read("e6502.ESP32/novahost/device_manager.h")
    bridge = read("e6502.ESP32/novahost/fpga_bridge.cpp")
    event_reader = read("e6502.ESP32/novahost/fio_event_reader.h")
    novahost = read("e6502.ESP32/novahost/novahost.ino")

    for constant in [
        "CMD_GSAVE", "CMD_GLOAD", "CMD_SIDPLAY", "CMD_MIDPLAY",
        "CMD_SFLOAD", "CMD_TSAVE", "CMD_TLOAD", "CMD_FORMAT", "CMD_PWD",
    ]:
        check(f"ESP dispatcher defines {constant}", constant in header)

    dispatch_checks = {
        "ESP handles GSAVE": "case CMD_GSAVE" in dispatcher and "handle_gsave();" in dispatcher,
        "ESP handles GLOAD": "case CMD_GLOAD" in dispatcher and "handle_gload();" in dispatcher,
        "ESP handles PWD": "case CMD_PWD" in dispatcher and "handle_pwd();" in dispatcher,
        "ESP registers SIDPLAY as deliberate SD command": 'handle_unsupported_sd_command("SIDPLAY")' in dispatcher,
        "ESP registers MIDPLAY as deliberate SD command": 'handle_unsupported_sd_command("MIDPLAY")' in dispatcher,
        "ESP registers SFLOAD as deliberate SD command": 'handle_unsupported_sd_command("SFLOAD")' in dispatcher,
        "ESP registers TSAVE as deliberate SD command": 'handle_unsupported_sd_command("TSAVE")' in dispatcher,
        "ESP registers TLOAD as deliberate SD command": 'handle_unsupported_sd_command("TLOAD")' in dispatcher,
        "ESP registers FORMAT as deliberate SD command": 'handle_unsupported_sd_command("FORMAT")' in dispatcher,
        "unsupported SD commands still finish through FIO error status": "void FioDispatcher::handle_unsupported_sd_command" in dispatcher
        and "respond_err(ERR_IO);" in dispatcher,
        "GSAVE reads VGC memory and writes NDI chunks": "readVgcBlock(space" in dispatcher
        and "write_file_chunk_by_index(new_idx" in dispatcher,
        "GLOAD reads NDI chunks and writes VGC memory": "read_file_chunk_by_index(idx" in dispatcher
        and "pokeVgcBlock(space" in dispatcher,
        "VGC FIO validates canonical VGC spaces": "case 0x01: return 4000" in dispatcher
        and "case 0x03: return 64000" in dispatcher
        and "case 0x06: return 32768" in dispatcher,
        "PWD has tracked current paths": "current_path(int slot)" in device_manager,
        "FPGA bridge drain preserves async FIO events": "_drainHandler(_drainUser" in bridge
        and "onDrainByteStatic" in event_reader
        and "fpgaBridge.onDrainByte" in novahost,
        "FIO command register is polled as event-loss fallback": "void FioDispatcher::poll_pending()" in dispatcher
        and "_bridge.peek(BANK_BASE + OFF_CMD" in dispatcher
        and "fioDispatcher.poll_pending();" in novahost,
    }
    for name, ok in dispatch_checks.items():
        check(name, ok)


def test_runtime_autoboot_contract() -> None:
    nova_inc = read("ehbasic/lib/nova.inc")
    fio = read("ehbasic/lib/fio.s")
    constants = read("e6502.Avalonia/Hardware/VgcConstants.cs")
    controller = read("e6502.Avalonia/Hardware/FileIoController.cs")
    composite = read("e6502.Avalonia/Hardware/CompositeBusDevice.cs")
    storage_dm = read("e6502.Storage/DeviceManager.cs")
    dispatcher_h = read("e6502.ESP32/novahost/fio_dispatcher.h")
    dispatcher = read("e6502.ESP32/novahost/fio_dispatcher.cpp")
    esp_dm_h = read("e6502.ESP32/novahost/device_manager.h")
    esp_dm = read("e6502.ESP32/novahost/device_manager.cpp")
    novahost = read("e6502.ESP32/novahost/novahost.ino")
    unit_dm = read("e6502UnitTests/DeviceManagerTests.cs")
    unit_fio = read("e6502UnitTests/FileIoControllerTests.cs")
    unit_rom = read("e6502UnitTests/RomSwapTests.cs")
    ozmoo_make = read("examples/ozmoo/Makefile")
    ozmoo_auto = read("examples/ozmoo/src/autoboot.s")
    ozmoo_runtime = read("examples/ozmoo/src/runtime.s")

    checks = {
        "BASIC exposes primary runtime ROM swap label": "ROMSWAP_PRIMARY" in nova_inc
        and "ROMSWAP_PRIMARY   = ROMSWAP_BASIC" in nova_inc,
        "BASIC exposes LOADRUNTIME command ID": "FIO_CMD_LOADRUNTIME = $28" in nova_inc,
        "shared FIO exports runtime loader": ".export fio_load_runtime" in fio
        and "LDA   #FIO_CMD_LOADRUNTIME" in fio,
        "emulator command constants match": "FioCmdLoadRuntime = 0x28" in constants
        and "RomSwapPrimary  = RomSwapBasic" in constants
        and "RomSize           = 0x4000" in constants,
        "emulator FIO handles runtime loading": "case VgcConstants.FioCmdLoadRuntime:" in controller
        and "DoLoadRuntime();" in controller
        and "data.Length != VgcConstants.RomSize" in controller,
        "emulator can replace the primary runtime ROM": "loadRuntimeRom: LoadPrimaryRuntimeRom" in composite
        and "Array.Copy(data, _basicRom, VgcConstants.RomSize)" in composite,
        "shared storage selects boot device FD-before-HD": 'SlotOrder = ["FD0", "FD1", "FD2", "FD3", "HD0", "HD1"]' in storage_dm
        and "SelectBootDevice()" in storage_dm,
        "ESP dispatcher defines runtime command": "CMD_LOADRUNTIME = 0x28" in dispatcher_h
        and "RUNTIME_ROM_BYTES = 16 * 1024" in dispatcher_h,
        "ESP dispatcher handles runtime command": "case CMD_LOADRUNTIME: handle_load_runtime(); break;" in dispatcher
        and "handle_load_runtime()" in dispatcher
        and "_bridge.loadRom(0" in dispatcher,
        "ESP load mirrors BAS-before-BIN resolution": "find_load_entry" in dispatcher
        and '"%s.bas"' in dispatcher
        and '"%s.bin"' in dispatcher,
        "ESP auto-mounts floppies and chooses boot slot": "auto_mount_fds()" in esp_dm_h
        and "select_boot_slot() const" in esp_dm_h
        and "int DeviceManager::auto_mount_fds()" in esp_dm
        and "int DeviceManager::select_boot_slot() const" in esp_dm
        and "FD0, FD1, FD2, FD3, HD0, HD1" in esp_dm,
        "NovaHost mounts FD before HD and sets default boot slot": "auto_mount_fds();" in novahost
        and "auto_mount_hds();" in novahost
        and "set_default_slot(boot_slot)" in novahost,
        "unit tests cover boot order": "FindAutoboot_PrefersInsertedFloppyOverHardDrive" in unit_dm
        and "SelectBootDevice_PrefersInsertedFloppyWhenNoAutobootExists" in unit_dm,
        "unit tests cover runtime load command": "LoadRuntime_LoadsExact16KImageIntoPrimaryRuntime" in unit_fio,
        "unit tests cover primary ROM swap alias": "WriteRomSwapPrimary_SelectsPrimaryRuntimeRom" in unit_rom,
        "Ozmoo example builds launcher plus runtime": "AUTOBOOT := $(BUILD_DIR)/AUTOBOOT.bin" in ozmoo_make
        and "RUNTIME := $(BUILD_DIR)/ozmoo.bin" in ozmoo_make
        and "IMAGE ?= $(DIST_DIR)/fd0.ndi" in ozmoo_make
        and "--runtime" in ozmoo_make,
        "Ozmoo launcher replaces BASIC runtime": "fio_load_runtime" in ozmoo_auto
        and "STA REG_ROMSWAP" in ozmoo_auto
        and "JMP ($FFFC)" in ozmoo_auto,
        "Ozmoo runtime loads story into XRAM": "JMP xram_xload" in ozmoo_runtime
        and '.byte "story.bin"' in ozmoo_runtime,
    }
    for name, ok in checks.items():
        check(name, ok)


def main() -> int:
    tests = [
        ("serial commands", test_serial_commands),
        ("REST routes", test_rest_routes),
        ("password redaction", test_password_redaction),
        ("host-status LED contract", test_host_status_led_contract),
        ("FIO clear-error contract", test_fio_clear_error_contract),
        ("FIO SD dispatch contract", test_fio_sd_dispatch_contract),
        ("runtime autoboot contract", test_runtime_autoboot_contract),
    ]
    failed = 0
    for name, fn in tests:
        print(f"\nTest: {name}")
        try:
            fn()
        except AssertionError:
            failed += 1
    print(f"\n{len(tests) - failed} passed, {failed} failed")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
