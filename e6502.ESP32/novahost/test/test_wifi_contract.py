#!/usr/bin/env python3
"""Static contract tests for NovaHost WiFi management.

These tests intentionally avoid Arduino or RF dependencies. They verify the
public management contract that has to remain stable across refactors:

- USB serial bootstrap commands exist.
- Management TCP endpoints are routed.
- Management status redacts the password and exposes only passwordSet.
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


def test_management_routes() -> None:
    src = read("e6502.ESP32/novahost/sd_http_server.cpp")
    header = read("e6502.ESP32/novahost/sd_http_server.h")
    debug = read("e6502.ESP32/novahost/debug_server.cpp")
    mgmt = read("e6502.ESP32/novahost/management_server.cpp")
    mgmt_header = read("e6502.ESP32/novahost/management_server.h")
    novahost = read("e6502.ESP32/novahost/novahost.ino")
    nova_cli = read("e6502.Nova/Program.cs")
    nova_web = read("e6502.Nova/NovaWebServer.cs")
    nova_mgmt = read("e6502.NovaHost/NovaHostManagementClient.cs")

    checks = {
        "HTTP documents only the health endpoint": 'GET /health -> NovaHost liveness' in header
        and "everything else" not in header,
        "HTTP non-health requests are just not found": 'send_error(client, 404, "not found")' in src
        and "use management tcp" not in src
        and 'case 410: return "Gone"' not in src,
        "GET /health exposes host-status flags": '\\"hostStatusHex\\"',
        "GET /health exposes HTTP task status": '\\"http\\":{\\"taskRunning\\"',
        "firmware no longer serves embedded control panel": 'strcmp(url, "/") == 0 || strcmp(url, "/control") == 0' not in src
        and "send_control_panel" not in src
        and "control_panel_page" not in src
        and "embedded NovaHost control panel" not in header,
        "old embedded REST handlers are deleted": "handle_wifi" not in src
        and "handle_drives" not in src
        and "handle_get" not in src
        and "handle_put" not in src
        and "handle_delete" not in src
        and "handle_status" not in src
        and "handle_reboot" not in src
        and "handle_vm_reset" not in src
        and "path_after_sd" not in src
        and "parse_drive_action" not in src
        and "read_body_to_file" not in src
        and "read_body_to_string" not in src
        and "write_json_string" not in src,
        "management VM reset calls cold boot helper": "handle_vm_reset" in mgmt
        and "novaVmReset()" in mgmt,
        "debug TCP no longer exposes VM reset": '"vm_reset"' not in debug
        and "cmdVmReset" not in debug,
        "VM reset performs a cold splash boot": "bool novaVmReset()" in novahost
        and "VM cold boot requested via management" in novahost
        and "showBootSplash()" in novahost.split("bool novaVmReset()", 1)[1].split("uint8_t novaHostStatusFlags", 1)[0]
        and "loadRomsToFPGA()" in novahost.split("bool novaVmReset()", 1)[1].split("uint8_t novaHostStatusFlags", 1)[0]
        and "BOOT_PHASE_SPLASH" in novahost.split("bool novaVmReset()", 1)[1].split("uint8_t novaHostStatusFlags", 1)[0]
        and "BOOT_PHASE_ROM_LOAD" in novahost.split("bool novaVmReset()", 1)[1].split("uint8_t novaHostStatusFlags", 1)[0],
        "HTTP request line timeout is bounded": "REQUEST_LINE_TIMEOUT_MS",
        "HTTP header line timeout is bounded": "HEADER_LINE_TIMEOUT_MS",
        "management connect action": 'action == "connect"' in mgmt,
        "management disconnect action": 'action == "disconnect"' in mgmt,
        "management reconnect action": 'action == "reconnect"' in mgmt,
        "management forget action": 'action == "forget"' in mgmt,
        "management drive mount persistence preserves existing slots": 'doc["mounts"].as<JsonObject>()' in mgmt
        and 'if (mounts.isNull())' in mgmt
        and 'mounts = doc["mounts"].to<JsonObject>()' in mgmt,
        "management drive mount uses shared storage lock": "handle_mount_drive" in mgmt
        and "novaBeginStorageMutation()" in mgmt.split("void ManagementServer::handle_mount_drive", 1)[1].split("void ManagementServer::handle_unmount_drive", 1)[0]
        and "novaEndStorageMutation()" in mgmt.split("void ManagementServer::handle_mount_drive", 1)[1].split("void ManagementServer::handle_unmount_drive", 1)[0]
        and '"storage_busy"' in mgmt.split("void ManagementServer::handle_mount_drive", 1)[1].split("void ManagementServer::handle_unmount_drive", 1)[0],
        "management drive unmount uses shared storage lock": "handle_unmount_drive" in mgmt
        and "novaBeginStorageMutation()" in mgmt.split("void ManagementServer::handle_unmount_drive", 1)[1].split("void ManagementServer::handle_runtime_config", 1)[0]
        and "novaEndStorageMutation()" in mgmt.split("void ManagementServer::handle_unmount_drive", 1)[1].split("void ManagementServer::handle_runtime_config", 1)[0]
        and '"storage_busy"' in mgmt.split("void ManagementServer::handle_unmount_drive", 1)[1].split("void ManagementServer::handle_runtime_config", 1)[0],
        "management uploads pause shared FPGA SPI traffic": "novaBeginStorageMutation()" in mgmt
        and "novaEndStorageMutation()" in mgmt
        and "lockSharedBus()" in novahost,
        "management TCP server listens on 6504": "uint16_t port = 6504" in mgmt_header
        and "ManagementServer managementServer" in novahost
        and "managementServer.begin()" in novahost
        and "managementServer.loop()" in novahost,
        "management protocol uses NVH1 framed CBOR": 'memcmp(header, "NVH1", 4)' in mgmt
        and "PROTOCOL_VERSION = 1" in mgmt_header
        and "CborWriter" in mgmt
        and "CborReader" in mgmt
        and "MAX_RAW_BYTES = 16384" in mgmt_header,
        "management protocol covers control center parity": "CMD_GET_STATUS" in mgmt_header
        and "CMD_LIST_DIRECTORY" in mgmt_header
        and "CMD_READ_FILE_CHUNK" in mgmt_header
        and "CMD_WRITE_FILE_BEGIN" in mgmt_header
        and "CMD_MOUNT_DRIVE" in mgmt_header
        and "CMD_SET_RUNTIME_CONFIG" in mgmt_header
        and "CMD_VM_RESET" in mgmt_header
        and "CMD_HOST_REBOOT" in mgmt_header
        and "CMD_WIFI_SCAN" in mgmt_header
        and "CMD_WIFI_CONFIG" in mgmt_header
        and "CMD_WIFI_ACTION" in mgmt_header,
        "management uploads use shared storage lock": "novaBeginStorageMutation()" in mgmt
        and "novaEndStorageMutation()" in mgmt
        and "handle_write_chunk" in mgmt
        and "handle_write_commit" in mgmt,
    }
    for name, needle in checks.items():
        check(name, needle if isinstance(needle, bool) else needle in src)
    check("Nova CLI owns the browser control center",
          '"webserver" or "web" => DoWebServer' in nova_cli
          and "NovaWebServer.Run" in nova_cli
          and "--no-open" in nova_cli
          and "server.Start();" in nova_web
          and "TryOpenBrowser(server.Url)" in nova_web)
    check("local webserver uses management TCP for expected device actions",
          '"/api/status"' in nova_web
          and '"/api/inventory"' in nova_web
          and "NovaHostManagementClient" in nova_web
          and "GetStatusAsync" in nova_web
          and "ListDirectoryAsync" in nova_web
          and "WriteFileAsync" in nova_web
          and "MountDriveAsync" in nova_web
          and "RuntimeSetAsync" in nova_web
          and '"/api/runtime/package"' in nova_web
          and "ZipArchive" in nova_web
          and '"/api/vm-reset"' in nova_web
          and '"/api/reboot"' in nova_web
          and '"/api/sd"' in nova_web
          and '"/events"' in nova_web)
    check("nova management client uses matching frame and CBOR primitives",
          '"NVH1"' in nova_mgmt
          and "HeaderBytes = 24" in nova_mgmt
          and "DefaultPort = 6504" in nova_mgmt
          and "CborLite" in nova_mgmt
          and "WriteFileChunk" in nova_mgmt
          and "ReadFileChunk" in nova_mgmt
          and "WifiConfig" in nova_mgmt)
    check("standalone nova CLI uses management TCP for remote control",
          "NovaHostManagementClient" in nova_cli
          and "RunManagement" in nova_cli
          and "HttpClient" not in nova_cli
          and "HttpGet" not in nova_cli
          and "HttpPost" not in nova_cli
          and "HttpPut" not in nova_cli)
    check("local webserver exposes disk/runtime/library controls",
          'id="slotGrid"' in nova_web
          and 'id="diskList"' in nova_web
          and 'id="runtimeSelect"' in nova_web
          and 'id="runtimePackage"' in nova_web
          and 'id="deployRuntimeBtn"' in nova_web
          and 'id="libraryList"' in nova_web
          and 'Host reboot to apply' in nova_web)
    check("HTTP response writes are bounded",
          "RESPONSE_WRITE_TIMEOUT_MS" in header
          and "availableForWrite()" in src
          and "client.write(data + off, chunk)" in src
          and "send_json" in src)
    check("management file downloads are chunked",
          "handle_read_file_chunk" in mgmt
          and "CMD_READ_FILE_CHUNK" in mgmt_header
          and "DownloadFileAsync" in nova_mgmt)
    check("HTTP handlers avoid unbounded client print helpers",
          "client.print(" not in src
          and "client.printf(" not in src
          and "client.flush(" not in src)


def test_password_redaction() -> None:
    mgmt = read("e6502.ESP32/novahost/management_server.cpp")
    status_body = mgmt.split('w.text("wifi");', 1)[1]
    status_body = status_body.split("char audio", 1)[0]

    check("management status exposes passwordSet", '"passwordSet"' in status_body)
    check("management status does not emit password field", '"password");' not in status_body)
    check("management config accepts password updates", 'key == "password"' in mgmt)


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
        "assign leds = led_operator_mode ? operator_leds :" in fpga
        and "music_leds_active ? music_leds :" in fpga
        and "user_leds;" in fpga,
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


def test_fpga_spi_bridge_contract() -> None:
    novahost = read("e6502.ESP32/novahost/novahost.ino")
    bridge_h = read("e6502.ESP32/novahost/fpga_bridge.h")
    bridge = read("e6502.ESP32/novahost/fpga_bridge.cpp")
    event_reader_h = read("e6502.ESP32/novahost/fio_event_reader.h")
    event_reader = read("e6502.ESP32/novahost/fio_event_reader.cpp")
    fpga_top = read("e6502.FPGA/fpga/fpga_top.sv")
    fpga_make = read("e6502.FPGA/fpga/Makefile")
    lpf = read("e6502.FPGA/fpga/ulx3s.lpf")
    spi_slave = read("e6502.FPGA/rtl/debug_spi_slave.sv")
    spi_test = read("e6502.FPGA/test/test_debug_spi_slave.sv")

    checks = {
        "NovaHost defines shared ULX3S SPI pins": "#define SD_SPI_SCK_PIN  14" in novahost
        and "#define SD_SPI_MISO_PIN 2" in novahost
        and "#define SD_SPI_MOSI_PIN 15" in novahost
        and "#define SD_CS_PIN       13" in novahost
        and "#define FPGA_SPI_CS_PIN 4" in novahost
        and "#define FPGA_SPI_HZ     40000000" in novahost,
        "NovaHost initializes one shared SPI bus": "void initSharedSpiBus()" in novahost
        and "SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_CS_PIN)" in novahost,
        "NovaHost bridge is SPI-only": "FpgaBridge fpgaBridge;" in novahost
        and "FPGA_SERIAL" not in novahost
        and "FpgaBridge(HardwareSerial" not in bridge_h
        and "_serial.write" not in bridge,
        "NovaHost bridge knows SD peer CS and holds it high": "beginSpi(SPI, FPGA_SPI_CS_PIN, FPGA_SPI_HZ, SD_CS_PIN)" in novahost
        and "_spiPeerCsPin" in bridge_h
        and "digitalWrite(_spiPeerCsPin, HIGH);" in bridge,
        "ESP bridge writes use explicit SPI WRITE transaction": "SPI_WRITE_OP = 0x57" in bridge_h
        and "_spi->transfer(SPI_WRITE_OP);" in bridge,
        "ESP bridge uses unpaced bulk SPI writes for payloads": "_spi->transferBytes(data, nullptr" in bridge
        and "SDRAM_STREAM_MAX_BYTES = 256" in bridge_h
        and "pokeSdramStreamChunk" in bridge
        and "writeBytesSdramPaced" not in bridge,
        "ESP bridge chunks WTS event streams": "WTS_EVENT_STREAM_MAX_BYTES = 60" in bridge_h
        and "writeWtsEventsChunk" in bridge
        and "writeBytesBulk(header, 3, data, count)" in bridge,
        "ESP bridge reads use token-framed SPI READ transaction": "SPI_READ_OP = 0x52" in bridge_h
        and "SPI_TOKEN_DATA = 0x01" in bridge_h
        and "spiReadSettings()" in bridge_h
        and "_spi->transfer(SPI_READ_OP);" in bridge
        and "uint8_t token = _spi->transfer(0x00);" in bridge,
        "ESP bridge can use separate write/read SPI clocks": "spiWriteSettings()" in bridge_h
        and "spiControlWriteSettings()" in bridge_h
        and "_spiReadHz" in bridge_h
        and "#define FPGA_SPI_READ_HZ (80000000UL / 3UL)" in bridge_h
        and "actualWriteHz" in bridge_h,
        "FIO/NIC event reader is transport-agnostic": "HardwareSerial" not in event_reader_h
        and "poll()" not in event_reader_h
        and "_serial" not in event_reader,
        "main loop drains events through the SPI bridge": "fpgaBridge.drain();" in novahost
        and "fioEventReader.poll();" not in novahost,
        "debug and SD HTTP services wait while boot owns shared SPI": "if (novaFpgaBridgeAvailable()) {" in novahost
        and "debugServer.loop();" in novahost
        and "sdHttpServer.loop();" in novahost,
        "FPGA board top routes debug bridge to SPI only": "assign dbg_rx_data = dbg_spi_rx_data;" in fpga_top
        and "assign dbg_rx_valid = dbg_spi_rx_valid;" in fpga_top
        and ".RX_ADDR_WIDTH(12)" in fpga_top
        and ".TX_ADDR_WIDTH(9)" in fpga_top
        and ".tx_start     (dbg_tx_start)" in fpga_top
        and "debug_uart_rx #(" not in fpga_top
        and "debug_uart_tx #(" not in fpga_top,
        "FPGA synthesis sources do not include ESP debug UART fallback": "RTL_BRIDGE" in fpga_make
        and "debug_spi_slave.sv" in fpga_make
        and "debug_uart_rx.sv" not in fpga_make
        and "debug_uart_tx.sv" not in fpga_make,
        "ULX3S SPI constraints use SD pins with FPGA CS on D1": 'SITE "H2"' in lpf
        and 'SITE "J1"' in lpf
        and 'SITE "J3"' in lpf
        and 'SITE "H1"' in lpf
        and 'FREQUENCY PORT "esp_spi_sck" 40 MHZ;' in lpf,
        "FPGA SPI slave protocol is command-framed": "WRITE_OP = 8'h57" in spi_slave
        and "READ_OP = 8'h52" in spi_slave
        and "READ_TOKEN_DATA = 8'h01" in spi_slave,
        "FPGA SPI test protects dummy-read and 0xA5 payload cases": "read polling did not enqueue dummy MOSI" in spi_test
        and "0xA5 payload survives framing" in spi_test,
    }
    for name, ok in checks.items():
        check(name, ok)


def test_fio_clear_error_contract() -> None:
    basic = read("software/languages/ehbasic/basic.asm")
    # The FIOCLR helper now lives in its own object (fio_clear_error.s) so ROMs
    # that only need FIOCLR avoid pulling the full fio.s body; fio_exec stays in
    # fio.s. The contract therefore spans both files.
    fio = read("software/runtime/asm/fio.s") + read("software/runtime/asm/fio_clear_error.s")
    nova_inc = read("software/runtime/asm/nova.inc")
    dispatcher_h = read("e6502.ESP32/novahost/fio_dispatcher.h")
    dispatcher_cpp = read("e6502.ESP32/novahost/fio_dispatcher.cpp")
    constants = read("e6502.Avalonia/Hardware/VgcConstants.cs")
    controller = read("e6502.Avalonia/Hardware/FileIoController.cs")
    unit_tests = read("e6502UnitTests/FileIoControllerTests.cs")
    integration = read("software/tests/integration/fio.6502")

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
        "BASIC command dispatches fio_clear_error": "LAB_FIOCLR" in basic
        and ("JSR   fio_clear_error" in basic or "JMP   fio_clear_error" in basic),
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
    ndi_image = read("e6502.ESP32/novahost/ndi_image.h")
    bridge = read("e6502.ESP32/novahost/fpga_bridge.cpp")
    event_reader = read("e6502.ESP32/novahost/fio_event_reader.h")
    novahost = read("e6502.ESP32/novahost/novahost.ino")
    constants = read("e6502.Avalonia/Hardware/VgcConstants.cs")
    controller = read("e6502.Avalonia/Hardware/FileIoController.cs")
    nova_inc = read("software/runtime/asm/nova.inc")
    fio = read("software/runtime/asm/fio.s")
    libfiles = read("software/runtime/asm/libfiles.inc")
    files_module = read("software/modules/files/files.s")
    forth_file = read("software/languages/novaforth/forth/lib/file.4th")
    novaforth = read("software/languages/novaforth/novaforth.s")

    for constant in [
        "CMD_GSAVE", "CMD_GLOAD", "CMD_SIDPLAY", "CMD_SIDSTOP", "CMD_MIDPLAY",
        "CMD_MIDSTOP", "CMD_SFLOAD", "CMD_FORMAT", "CMD_PWD",
        "CMD_FOPEN", "CMD_FCREATE", "CMD_FCLOSE", "CMD_FREAD", "CMD_FWRITE",
        "CMD_FSEEK", "CMD_FTELL", "CMD_FSIZE", "CMD_FRESIZE", "CMD_FFLUSH",
        "CMD_FSTATUS", "CMD_FDELETE", "CMD_FRENAME",
    ]:
        check(f"ESP dispatcher defines {constant}", constant in header)

    dispatch_checks = {
        "ESP handles GSAVE": "case CMD_GSAVE" in dispatcher and "handle_gsave();" in dispatcher,
        "ESP handles GLOAD": "case CMD_GLOAD" in dispatcher and "handle_gload();" in dispatcher,
        "ESP handles PWD": "case CMD_PWD" in dispatcher and "handle_pwd();" in dispatcher,
        "ESP handles SIDPLAY": "case CMD_SIDPLAY" in dispatcher and "handle_sidplay();" in dispatcher,
        "ESP handles SIDSTOP": "case CMD_SIDSTOP" in dispatcher and "handle_sidstop();" in dispatcher,
        "ESP SIDPLAY uses ESP-side SID VM": "nova_sid::SidVm _sid_vm" in header
        and "runInit((uint8_t)(song - 1))" in dispatcher
        and "runPlayFrame()" in dispatcher,
        "ESP SIDPLAY uses sparse SID payload memory, not Nova CPU RAM": "_sid_vm.loadPayload" in dispatcher
        and "_bridge.loadRam((uint16_t)(sid.loadAddress + off)" not in dispatcher,
        "ESP SIDPLAY batches trapped SID register writes over SPI": "on_sid_write_static" in dispatcher
        and "flush_sid_writes()" in dispatcher
        and "_bridge.pokeMulti(_sid_write_addrs" in dispatcher,
        "ESP SIDPLAY programs SID model/clock config": "nova_sid::sid_fpga_config(sid)" in dispatcher
        and "configure_sid(_bridge, nova_sid::sid_fpga_config(sid))" in dispatcher,
        "ESP SIDSTOP clears HDMI diagnostic tone": "configure_sid(_bridge, 0)" in dispatcher
        and "SID_CFG_ADDR" in dispatcher,
        "ESP SID silence preserves Nova per-voice volume": "off <= 0x18" in dispatcher
        and "off = 0x1D" in dispatcher
        and "SID1_BASE + off), 0x0F" in dispatcher,
        "ESP handles MIDPLAY": "case CMD_MIDPLAY" in dispatcher and "handle_midplay();" in dispatcher,
        "ESP handles MIDSTOP": "case CMD_MIDSTOP" in dispatcher and "handle_midstop();" in dispatcher,
        "ESP MIDPLAY requires timestamped WTS event streaming": "start_music_wts_event_stream()" in dispatcher
        and "queue_music_wts_events()" in dispatcher
        and "supportsWtsEventStream()" in bridge
        and "writeWtsEvents" in bridge,
        "ESP MIDPLAY bounds expanded WTS note events": "NMS_WTS_NOTE_ON_RECORDS = 41" in dispatcher
        and "WTS event expansion overflow" in dispatcher
        and "out_capacity" in dispatcher,
        "ESP MIDPLAY batches NMS event reads": "_midi_read_cache" in header
        and "cache_bytes = sizeof(_midi_read_cache)" in dispatcher
        and "_midi_read_cache + cache_off" in dispatcher,
        "ESP WTS sample window uses reclaimed SDRAM": "WTS_SAMPLE_SDRAM_BASE = 0x082000UL" in dispatcher
        and "WTS_SAMPLE_SDRAM_LIMIT = 0x1000000UL" in dispatcher
        and "SID_CURVE_BASE   = 0x080000" in novahost,
        "ESP MIDPLAY consumes precompiled Nova music streams": "find_music_entry" in dispatcher
        and '".nms"' in dispatcher
        and "read_music_stream_header" in dispatcher
        and "NMS_MAGIC" in dispatcher
        and "build_music_wts_event" in dispatcher
        and "soundfont missing instrument" in dispatcher,
        "ESP mirrors hosted music state into Nova music registers": "MUSIC_MIRROR_BASE     = 0xBA50" in dispatcher
        and "update_music_mirror(false)" in dispatcher
        and "_bridge.pokeBlock(MUSIC_MIRROR_BASE" in dispatcher
        and "MUSIC_STATUS_MUSIC | MUSIC_STATUS_WTS" in dispatcher
        and "MUSIC_STATUS_MUSIC | MUSIC_STATUS_SID" in dispatcher,
        "ESP mirrors SID notes from trapped writes": "capture_sid_mirror_write(addr, value)" in dispatcher
        and "sid_freq_to_midi" in dispatcher
        and "_sid_mirror_ctrl" in header,
        "ESP mirrors WTS notes from the NMS event timeline": "enqueue_midi_visual_event" in dispatcher
        and "update_midi_visual_notes" in dispatcher
        and "_midi_visual_notes[event.voice]" in dispatcher
        and "midi_elapsed_audio_frames" in dispatcher
        and "MusicMirrorBytes = 19" in header,
        "ESP MIDPLAY fails loud instead of falling back": "ensure_wts_bank_loaded(slot)" in dispatcher
        and "Missing soundfont" in dispatcher
        and "hardware WTS event stream unavailable" in dispatcher
        and "resident wave fallback" not in dispatcher
        and "loaded but not ESP-resident" not in dispatcher
        and "fill_midi_audio_fifo" not in dispatcher
        and "render_midi_pcm_frames" not in dispatcher
        and "writeAudioPcm" not in dispatcher,
        "ESP WTS auto-load uses only compact full-instrument defaults": "WTS_AUTO_SOUNDFONT_PRIORITY" in dispatcher
        and "timgm6mb.nsfb" in dispatcher
        and "florestan-basic.nsfb" in dispatcher
        and "GeneralUser_GS.nsfb" in dispatcher
        and "is_skipped_auto_soundfont" in dispatcher
        and "MAX_REGIONS = 1408" in header
        and "bohemian.nsfb" not in dispatcher
        and "stars-timgm6mb.nsfb" not in dispatcher,
        "ESP MIDPLAY uses hardware WTS only after probing the bitstream": "hardware_wts_available()" in dispatcher
        and "WTS_SOUNDFONT_STATUS" in dispatcher
        and "WTS_SIGNATURE" in dispatcher
        and "NOVAHOST_ENABLE_HARDWARE_WTS" in dispatcher
        and "hardware WTS unavailable in this bitstream" in dispatcher
        and "WTS event stream unsupported in this bitstream" in dispatcher,
        "ESP audio status exposes MIDI timing diagnostics": "pumpCalls" in dispatcher
        and "maxPumpGapUs" in dispatcher
        and "reset_midi_timing_metrics()" in dispatcher
        and "musicEventBytes" in dispatcher
        and "musicEventRead" in dispatcher
        and "_midi_max_pump_gap_us" in header
        and "_music_event_bytes" in header
        and "_music_event_read" in header,
        "ESP audio status waits briefly for coherent snapshots":
            "FioDispatcherStateGuard stateGuard(*this, pdMS_TO_TICKS(20));" in dispatcher,
        "ESP MIDPLAY runtime does not parse Standard MIDI files": "nova_midi" not in dispatcher
        and '#include "midi_engine.h"' not in header
        and "_midi_prefetch_timeline" not in header
        and "midi_prefetch_task_main" not in dispatcher,
        "ESP handles SFLOAD": "case CMD_SFLOAD:   handle_sfload();" in dispatcher,
        "ESP registers FORMAT as deliberate SD command": 'handle_unsupported_sd_command("FORMAT")' in dispatcher,
        "unsupported SD commands still finish through FIO error status": "void FioDispatcher::handle_unsupported_sd_command" in dispatcher
        and "respond_err(ERR_IO);" in dispatcher,
        "GSAVE reads VGC memory and writes NDI chunks": "readVgcBlock(space" in dispatcher
        and "write_file_chunk_by_index(new_idx" in dispatcher,
        "GLOAD reads NDI chunks and writes VGC memory": "read_file_chunk_by_index(idx" in dispatcher
        and "pokeVgcBlock(space" in dispatcher,
        "DIROPEN populates the first directory entry": "bool FioDispatcher::write_next_dir_entry()" in dispatcher
        and "void FioDispatcher::handle_dir_open()" in dispatcher
        and "if (write_next_dir_entry())" in dispatcher,
        "DIRREAD exposes 24-bit file sizes": "OFF_SIZE_2    = OFF_GSPACE" in header
        and "OFF_SIZE_2" in dispatcher
        and "(e.size_bytes >> 16)" in dispatcher,
        "low-level FILE command IDs match across host implementations": "FIO_CMD_FOPEN    = $2D" in nova_inc
        and "FIO_CMD_FRENAME  = $39" in nova_inc
        and re.search(r"FioCmdFOpen\s*=\s*0x2D", constants) is not None
        and re.search(r"FioCmdFRename\s*=\s*0x39", constants) is not None
        and "CMD_FOPEN    = 0x2D" in header
        and "CMD_FRENAME  = 0x39" in header,
        "ESP handles low-level FILE commands": "case CMD_FOPEN:    handle_fopen(false); break;" in dispatcher
        and "case CMD_FCREATE:  handle_fopen(true);  break;" in dispatcher
        and "case CMD_FCLOSE:   handle_fclose();     break;" in dispatcher
        and "case CMD_FREAD:    handle_fread();      break;" in dispatcher
        and "case CMD_FWRITE:   handle_fwrite();     break;" in dispatcher
        and "case CMD_FSEEK:    handle_fseek();      break;" in dispatcher
        and "case CMD_FTELL:    handle_ftell();      break;" in dispatcher
        and "case CMD_FSIZE:    handle_fsize();      break;" in dispatcher
        and "case CMD_FRESIZE:  handle_fresize();    break;" in dispatcher
        and "case CMD_FFLUSH:   handle_fflush();     break;" in dispatcher
        and "case CMD_FSTATUS:  handle_fstatus();    break;" in dispatcher
        and "case CMD_FDELETE:  handle_fdelete();    break;" in dispatcher
        and "case CMD_FRENAME:  handle_frename();    break;" in dispatcher,
        "ESP low-level FILE handles report 24-bit counts": "uint32_t result_size24() const" in header
        and "void FioDispatcher::write_size24" in dispatcher
        and "SetTransferSize24" in controller,
        "low-level FILE target bits match across host implementations": "FioFileTargetMask  = 0x30" in constants
        and "FioFileTargetXram  = 0x10" in constants
        and "FIO_FILE_TARGET_MASK  = $30" in nova_inc
        and "FIO_FILE_TARGET_XRAM  = $10" in nova_inc
        and "FILE_TARGET_MASK = 0x30" in header
        and "FILE_TARGET_XRAM = 0x10" in header,
        "low-level FILE handles can stream directly to XRAM": "FioFileTargetXram" in controller
        and "_xramWrite(dest + i" in controller
        and "_xramRead!(src + i)" in controller
        and "_bridge.pokeSdramStream(dest + off" in dispatcher
        and "_bridge.readSdramBlock(src_addr + off" in dispatcher,
        "NDI and host dispatch tag Forth source files": "FT_FORTH = 6" in ndi_image
        and 'strcasecmp(ext, ".4th")' in dispatcher
        and "FioDirTypeForth  = 0x06" in constants,
        "NDK exposes low-level FILE wrappers": "FILE_FOPEN     = $0F" in libfiles
        and "FILE_FRENAME   = $1B" in libfiles
        and "FILE_FN_COUNT  = $1C" in libfiles
        and "file_fopen" in files_module
        and "file_frename" in files_module
        and "fio_fopen:" in fio
        and "fio_frename:" in fio,
        "NovaForth keeps standard file words in source over a tiny ROM bridge": "word_nova_libcall" in novaforth
        and "NOVA-LIBCALL" in novaforth
        and ": OPEN-FILE" in forth_file
        and ": READ-LINE" in forth_file
        and "FILE-CALL" in forth_file,
        "VGC FIO validates canonical VGC spaces": "case 0x01: return 4000" in dispatcher
        and "case 0x02: return 4000" in dispatcher
        and "case 0x03: return 64000" in dispatcher
        and "case 0x04: return 32768" in dispatcher
        and "case 0x07: return 4000" in dispatcher,
        "PWD has tracked current paths": "current_path(int slot)" in device_manager,
        "FPGA bridge drain preserves async FIO events": "_drainHandler(_drainUser" in bridge
        and "onDrainByteStatic" in event_reader
        and "fpgaBridge.onDrainByte" in novahost,
        "FPGA bridge status receive consumes complete async event packets": "int eventType = recvByte();" in bridge
        and "_drainHandler(_drainUser, (uint8_t)eventType);" in bridge
        and "continue;" in bridge,
        "FIO command register is polled as event-loss fallback": "void FioDispatcher::poll_pending()" in dispatcher
        and "_bridge.peek(BANK_BASE + OFF_CMD" in dispatcher
        and "fioDispatcher.poll_pending();" in novahost,
    }
    for name, ok in dispatch_checks.items():
        check(name, ok)


def test_nic_command_sequence_contract() -> None:
    nova_inc = read("software/runtime/asm/nova.inc")
    nic_inc = read("software/runtime/asm/nic.inc")
    nic_runtime = read("software/runtime/asm/nic.s")
    constants = read("e6502.Avalonia/Hardware/VgcConstants.cs")
    fpga_nic = read("e6502.FPGA/rtl/nic.sv")
    dispatcher_h = read("e6502.ESP32/novahost/nic_dispatcher.h")
    dispatcher = read("e6502.ESP32/novahost/nic_dispatcher.cpp")

    checks = {
        "BASIC exposes NIC command sequence register": "NIC_CMDSEQ        = $A105" in nova_inc,
        "runtime documents NIC command sequence semantics": "writes NIC_CMDSHADOW first" in nic_inc
        and "increments NIC_CMDSEQ" in nic_inc
        and "then writes" in nic_inc
        and "NIC_CMD" in nic_inc,
        "shared NIC command helper stages shadow before sequence and command": re.search(
            r"STA\s+NIC_CMDSHADOW[\s\S]*INC\s+NIC_CMDSEQ[\s\S]*@seq_ok:[\s\S]*STA\s+NIC_CMD\s*\n",
            nic_runtime,
        )
        is not None,
        "emulator constants reserve NIC command sequence registers": "public const int NicCmdSeq         = 0xA105" in constants
        and "public const int NicCmdShadow      = 0xA106" in constants,
        "FPGA NIC register map reserves command sequence registers": "+05  NIC_CMDSEQ" in fpga_nic
        and "+06  NIC_CMDSHADOW" in fpga_nic,
        "NovaHost reads command sequence from NIC bank": "OFF_CMDSEQ    = 0x05" in dispatcher_h
        and "OFF_CMDSHADOW = 0x06" in dispatcher_h
        and "uint8_t seq = _bank[OFF_CMDSEQ]" in dispatcher
        and "_lastCommandSeq = seq" in dispatcher,
        "NovaHost recovers a command cleared before event service": "cmd = _bank[OFF_CMDSHADOW]" in dispatcher
        and "_bank[OFF_CMDSHADOW] != 0" in dispatcher,
        "NovaHost does not retry-clear a fresh command": "bool NicDispatcher::is_new_command" in dispatcher
        and "seq != _lastCommandSeq" in dispatcher
        and "_pending = true" in dispatcher,
        "NovaHost retries command clear synchronously first": "for (int attempt = 0; attempt < 3; attempt++)" in dispatcher
        and "_bridge.poke(BANK_BASE + OFF_CMD, 0)" in dispatcher,
        "NovaHost reads SEND payloads from FPGA NIC TX buffer": "nicReadTxBlock(0, wireCount, buf)" in dispatcher
        and "OFF_DMASTATUS = 0x14" in dispatcher_h
        and "DMAST_TX_READY" in dispatcher_h,
        "NovaHost SEND relies on write results instead of WiFiClient connected preflight": "!s.connected || len > MAX_MSG_SIZE" in dispatcher
        and "!s.connected || !s.client.connected() || len > MAX_MSG_SIZE" not in dispatcher,
        "NovaHost preserves buffered bytes after TCP close": "!s.client.connected() && s.client.available() == 0" in dispatcher,
        "NovaHost stages RECV payloads into FPGA NIC RX buffer": "nicWriteRxBlock(0, msg.data, wireCount)" in dispatcher
        and "OFF_HOSTCTRL  = 0x07" in dispatcher_h
        and "HOSTCTRL_RX_START" in dispatcher_h,
        "NovaHost waits for hardware NIC RX DMA completion": "wait_dma_complete(DMAST_RX_DONE)" in dispatcher
        and "DMAST_BUSY" in dispatcher_h
        and "DMAST_ERROR" in dispatcher_h,
        "NovaHost NIC path no longer pauses CPU for payload DMA": "send dma pause failed" not in dispatcher
        and "recv dma pause failed" not in dispatcher,
    }
    for name, ok in checks.items():
        check(name, ok)


def test_large_ndi_bam_contract() -> None:
    ndi_image = read("e6502.ESP32/novahost/ndi_image.cpp")

    checks = {
        "ESP NDI mount avoids duplicate large BAM allocation": "bam payload malloc failed" not in ndi_image
        and "uint8_t* payload" not in ndi_image
        and "_stream->read(_bam_bits, _bam_byte_count)" in ndi_image,
        "ESP NDI metadata flush streams BAM sectors": "uint8_t sector[SECTOR_SIZE]" in ndi_image
        and "while (written < bam_bytes_padded)" in ndi_image
        and "_stream->write(sector, SECTOR_SIZE)" in ndi_image
        and "ndi_malloc(bam_bytes_padded)" not in ndi_image,
    }
    for name, ok in checks.items():
        check(name, ok)


def test_runtime_autoboot_contract() -> None:
    nova_inc = read("software/runtime/asm/nova.inc")
    basic = read("software/languages/ehbasic/basic.asm")
    fio = read("software/runtime/asm/fio.s")
    rng = read("software/runtime/asm/rng.s")
    constants = read("e6502.Avalonia/Hardware/VgcConstants.cs")
    controller = read("e6502.Avalonia/Hardware/FileIoController.cs")
    composite = read("e6502.Avalonia/Hardware/CompositeBusDevice.cs")
    storage_dm = read("e6502.Storage/DeviceManager.cs")
    dispatcher_h = read("e6502.ESP32/novahost/fio_dispatcher.h")
    dispatcher = read("e6502.ESP32/novahost/fio_dispatcher.cpp")
    bridge = read("e6502.ESP32/novahost/fpga_bridge.cpp")
    debug = read("e6502.ESP32/novahost/debug_server.cpp")
    esp_dm_h = read("e6502.ESP32/novahost/device_manager.h")
    esp_dm = read("e6502.ESP32/novahost/device_manager.cpp")
    novahost = read("e6502.ESP32/novahost/novahost.ino")
    mgmt = read("e6502.ESP32/novahost/management_server.cpp")
    boot_json = read("e6502.ESP32/novahost/assets/config/boot.json")
    novahost_make = read("e6502.ESP32/novahost/Makefile")
    flash_stack = read("tools/flash-ulx3s-stack.sh")
    client = read("tools/novahost_client.py")
    logo_smoke = read("tools/run-novalogo-hardware-smoke.py")
    unit_dm = read("e6502UnitTests/DeviceManagerTests.cs")
    unit_fio = read("e6502UnitTests/FileIoControllerTests.cs")
    unit_rom = read("e6502UnitTests/RomSwapTests.cs")
    novaz_make = read("software/examples/novaz/Makefile")
    novaz_auto = read("software/examples/novaz/src/autoboot.s")
    novaz_runtime = read("software/examples/novaz/src/runtime.s")
    novaz_zstory = read("software/examples/novaz/src/zstory.s")
    novaz_zstory_inc = read("software/examples/novaz/src/zstory.inc")
    novaz_zvm6_inc = read("software/examples/novaz/src/zvm6.inc")
    novaz_zvm6 = read("software/examples/novaz/src/zvm6.s")
    novaz_save_load = read("software/examples/novaz/src/save_load_overlay.s")
    nvg_runtime = read("software/runtime/asm/nvg.s")
    nvg_inc = read("software/runtime/asm/nvg.inc")
    vgc_palette_inc = read("software/runtime/asm/vgc_palette.inc")
    vgc_palette = read("software/runtime/asm/vgc_palette.s")
    libabi_inc = read("software/runtime/asm/libabi.inc")
    xram_inc = read("software/runtime/asm/xram.inc")
    type_text = debug.split("void DebugServer::cmdTypeText", 1)[1]
    type_text = type_text.split("void DebugServer::cmdReadScreen", 1)[0]
    novaz_linefeed = novaz_runtime.split("nz_screen_linefeed:", 1)[1].split("nz_screen_flush_word:", 1)[0]
    novaz_pre_newline = novaz_zvm6.split("nz6_op_pre_newline_scroll:", 1)[1].split("nz6_op_cr_newline:", 1)[0]
    novaz_cr_newline = novaz_zvm6.split("nz6_op_cr_newline:", 1)[1].split("nz6_sample_cursor_pixel:", 1)[0]

    checks = {
        "BASIC exposes primary runtime ROM swap label": "ROMSWAP_PRIMARY" in nova_inc
        and "ROMSWAP_PRIMARY   = ROMSWAP_BASIC" in nova_inc,
        "BASIC exposes LOADRUNTIME command ID": "FIO_CMD_LOADRUNTIME = $28" in nova_inc,
        "BASIC exposes host RNG command ID": "FIO_CMD_RNG      = $2A" in nova_inc
        and "FIO_RNG0         = FIO_SRCL" in nova_inc,
        "BASIC RND is seeded from host RNG at cold start": "JSR   LAB_SEED_RND_HOST" in basic
        and "LDA   #FIO_CMD_RNG" in basic
        and "STA   Rbyte4" in basic,
        "shared FIO exports runtime loader": ".export fio_load_runtime" in fio
        and "LDA   #FIO_CMD_LOADRUNTIME" in fio,
        "shared RNG library wraps host command": ".export rng_get32" in rng
        and "LDA   #FIO_CMD_RNG" in rng,
        "shared NVG library exposes native image loader": "$(NOVA_ASM)/nvg.inc $(NOVA_ASM)/nvg.s" in read("software/languages/ehbasic/Makefile")
        and ".export nvg_load" in nvg_runtime
        and ".export nvg_draw" in nvg_runtime,
        "shared VGC palette library exposes fixed and custom palette helpers": "$(NOVA_ASM)/vgc_palette.inc $(NOVA_ASM)/vgc_palette.s" in read("software/languages/ehbasic/Makefile")
        and "VGC_CUSTOM_PALETTE_BYTES" in vgc_palette_inc
        and ".global vgc_set_palette_c64" in vgc_palette_inc
        and ".global vgc_set_palette_ega" in vgc_palette_inc
        and ".global vgc_set_palette_custom_xram" in vgc_palette_inc
        and "vgc_set_palette_c64:" in vgc_palette
        and "$AA,$FF,$66" in vgc_palette
        and "vgc_set_palette_ega:" in vgc_palette
        and "$55,$55,$FF" in vgc_palette
        and "vgc_set_palette_custom_xram:" in vgc_palette
        and "vgc_upload_palette_rgb_xram:" in vgc_palette
        and "STZ   VGC_PALIDX" in vgc_palette
        and "STA   VGC_PALDATA" in vgc_palette
        and "STA   VGC_PALETTE" not in vgc_palette,
        "NovaZ V6 uses shared VGC palette helpers": "JSR vgc_set_palette_ega" in novaz_zvm6
        and "JSR vgc_set_palette_custom_xram" in novaz_zvm6
        and "JSR vgc_upload_palette_rgb_xram" not in novaz_zvm6,
        "NovaZ runtime initializes with shared C64 palette and reset border": ".include \"vgc_palette.inc\"" in novaz_runtime
        and "JSR vgc_set_palette_c64" in novaz_runtime
        and "LDA #$0B" in novaz_runtime.split("init_video_colors:", 1)[1].split("setup_text_region:", 1)[0]
        and "STA VGC_BORDER" in novaz_runtime.split("init_video_colors:", 1)[1].split("setup_text_region:", 1)[0]
        and "STZ VGC_BORDER" not in novaz_runtime,
        "NovaZ V6 updates the physical border only on real background fills": "nz6_sync_border_to_bg:" in novaz_zvm6
        and "STA VGC_BORDER" in novaz_zvm6
        and "JSR nz6_sync_border_to_bg" in novaz_zvm6.split("nz6_op_reset:", 1)[1].split("; --- PICS.PAK index", 1)[0]
        and "JSR nz6_sync_border_to_bg" not in novaz_zvm6.split("nz6_apply_colour_style:", 1)[1].split("nz6_sync_border_to_bg:", 1)[0]
        and "JSR nz6_sync_border_to_bg" in novaz_zvm6.split("nz6_gfx_fill_bg:", 1)[1].split("nz6_gfx_clear:", 1)[0],
        # V6 newline scroll now scrolls the gfx and text planes atomically through
        # the VTEXT scroll hook (nz6_scroll_live_composite -> vtext_scroll_composite_up
        # -> VCMD_SCROLLMIXED). This replaces the old "pre-scroll graphics, then text"
        # two-pass approach and removes the tear between planes. The PRE_NEWLINE_SCROLL
        # opcode is retained as a now-empty ABI slot.
        "NovaZ V6 scrolls graphics and text atomically on newline scroll": "NZ6_OP_PRE_NEWLINE_SCROLL = $0B" in novaz_zvm6_inc
        and "JSR vtext_set_scroll_hook" in novaz_zvm6
        and "nz6_scroll_live_composite:" in novaz_zvm6
        and "JMP vtext_scroll_composite_up" in novaz_zvm6
        and "nz6_gfx_scroll_live" not in novaz_zvm6,
        "shared NVG library pages packed NVG2 through the blitter": "row-packed 4bpp pixels" in nvg_inc
        and "pager_load_current_file_page" in nvg_runtime
        and "blitter_start_gfx4_unpack" in nvg_runtime
        and "XRAM_NVG_STAGE_L" in nvg_runtime
        and "FIO_CMD_NVGLOAD" not in nvg_runtime,
        "shared NVG loader uses shared VGC palette helper": ".include \"vgc_palette.inc\"" in nvg_inc
        and "NVG_PALETTE_BYTES = VGC_CUSTOM_PALETTE_BYTES" in nvg_inc
        and "JSR   vgc_set_palette_custom_xram" in nvg_runtime
        and "STA   VGC_PALDATA" not in nvg_runtime
        and "STA   VGC_PALETTE" not in nvg_runtime,
        "shared XRAM layout keeps app allocations off the library shelf": "XRAM_USER_HEAP_PAGES = 1024" in xram_inc
        and "xmc_alloc_block / MEM_ALLOC" in xram_inc
        and "XRAM_NOVAZ_" not in xram_inc
        and "SHELF_BASE_H     = $06" in libabi_inc
        and "SHELF_N          = 4" in libabi_inc
        and "XRAM_NVG_STAGE_H     = $07" in xram_inc,
        "emulator command constants match": "FioCmdLoadRuntime = 0x28" in constants
        and "FioCmdRng        = 0x2A" in constants
        and "FioCmdNvgLoad    = 0x2B" in constants
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
        "ESP dispatcher defines hardware RNG command": "CMD_RNG      = 0x2A" in dispatcher_h
        and "void handle_rng();" in dispatcher_h,
        "ESP dispatcher defines NVG load command": "CMD_NVGLOAD  = 0x2B" in dispatcher_h
        and "void handle_nvgload();" in dispatcher_h,
        "ESP dispatcher handles runtime command": "case CMD_LOADRUNTIME:" in dispatcher
        and "handle_load_runtime()" in dispatcher
        and "pokeRomBlock(0" in dispatcher,
        "ESP dispatcher handles hardware RNG command": "case CMD_RNG:" in dispatcher
        and "handle_rng()" in dispatcher
        and "esp_random()" in dispatcher,
        "ESP dispatcher handles NVG2 load command": "case CMD_NVGLOAD:" in dispatcher
        and "handle_nvgload()" in dispatcher
        and "magic[3] != '2'" in dispatcher
        and "NVG_FLAG_PALETTE" in dispatcher
        and "NVG_FLAG_TRANSPARENT" in dispatcher
        and "VGC_PALETTE_DATA_ADDR" in dispatcher
        and "VGC_PALETTE_MODE_ADDR" not in dispatcher
        and "VGC_PALMODE_CUSTOM" not in dispatcher,
        "ESP load mirrors BAS-before-BIN resolution": "find_load_entry" in dispatcher
        and '"%s.bas"' in dispatcher
        and '"%s.bin"' in dispatcher,
        "ESP boot config controls mounted drives and chooses boot slot": "mountConfiguredDrives" in novahost
        and 'doc["mounts"]' in novahost
        and "select_boot_slot() const" in esp_dm_h
        and "int DeviceManager::select_boot_slot() const" in esp_dm
        and "FD0, FD1, FD2, FD3, HD0, HD1" in esp_dm,
        "staged boot config defines NovaLogo runtime": '"novalogo"' in boot_json
        and '"/roms/novalogo.bin"' in boot_json
        and '"novaforth"' in boot_json
        and '"/roms/novaforth.bin"' in boot_json,
        "staged SD assets include NovaLogo ROMs": "$(NOVALOGO)/novalogo.bin" in novahost_make
        and "$(NOVAFORTH)/novaforth.bin" in novahost_make
        and "sd-assets: ehbasic novalogo novaforth" in novahost_make,
        "stack deploy exposes NovaLogo post-deploy smoke gate": "NOVALOGO_SMOKE=1" in flash_stack
        and "run_novalogo_smoke" in flash_stack
        and "tools/run-novalogo-hardware-smoke.py" in flash_stack,
        "stack deploy refuses stale FPGA bitstreams by default": "BITSTREAM_FRESHNESS" in flash_stack
        and "ALLOW_STALE_BITSTREAM" in flash_stack
        and "check-bitstream-freshness.py" in flash_stack,
        "NovaLogo smoke covers DRAW corruption and turn-forward regressions": "I DON'T KNOW HOW TO DRAW" in logo_smoke
        and "expected only the turtle in graphics plane after DRAW" in logo_smoke
        and "FORWARD 20" in logo_smoke
        and "RIGHT 45" in logo_smoke
        and "FD 50 south after the second relative RT 90" in logo_smoke
        and "cardinal=x" in logo_smoke
        and "SETPOS [100 50]" in logo_smoke
        and "isolated pen-down move redrew from old center" in logo_smoke,
        "NovaLogo smoke chunks VGC reads to the hardware limit": "chunk = min(256, width - offset)" in logo_smoke
        and "offset += chunk" in logo_smoke,
        "NovaLogo smoke waits for command effects and a real prompt": "def run_logo_line_until" in logo_smoke
        and "wait_for_condition(description" in logo_smoke
        and "cursor_on_prompt" in logo_smoke
        and "cursor_x == 2" in logo_smoke
        and 'cursor_line.strip() == "?"' in logo_smoke
        and '"enabled" in cursor' in logo_smoke,
        "shared NovaHost client normalizes hardware/Avalonia VRAM response names": '"values" not in response' in client
        and '"data" in response' in client
        and 'response["values"] = response["data"]' in client,
        "ESP boot config uses FD-before-HD mount order": "DeviceManager::FD0, DeviceManager::FD1" in novahost
        and "DeviceManager::FD2, DeviceManager::FD3" in novahost
        and "DeviceManager::HD0, DeviceManager::HD1" in novahost,
        "ESP boot config clears stale logical mount pointers": "mounts[prefix] = \"\";" in novahost
        and "config_dirty = true;" in novahost
        and "writeBootConfig(doc)" in novahost,
        "NovaHost does not blindly mount root disk images": "auto_mount_fds();" not in novahost
        and "auto_mount_hds();" not in novahost
        and "set_default_slot(boot_slot)" in novahost,
        "management drive changes persist logical slot pointers": "persist_drive_mount_config" in mgmt
        and "load_drive_mount_config" in mgmt
        and 'mounts[prefix] = sd_path ? sd_path : ""' in mgmt
        and '"/%s.ndi"' not in mgmt
        and "configuredPath" in mgmt,
        "management drive changes refresh boot default": "int boot_slot = _dm.select_boot_slot()" in mgmt
        and "_dm.set_default_slot(boot_slot)" in mgmt
        and '"bootDefault"' in mgmt,
        "VM reset reselects boot default before ROM release": "VM cold boot default device=%s" in novahost
        and "deviceManager.select_boot_slot()" in novahost.split("bool novaVmReset()", 1)[1].split("g_fpga_bridge_owned_by_boot = true", 1)[0]
        and "deviceManager.set_default_slot(boot_slot)" in novahost.split("bool novaVmReset()", 1)[1].split("g_fpga_bridge_owned_by_boot = true", 1)[0],
        "debug text injection normalizes LF to BASIC Enter": "void DebugServer::cmdTypeText" in debug
        and "ch == '\\n'" in debug
        and "ch = '\\r'" in debug,
        "debug text injection uses FIFO-backed bulk key streaming": "void DebugServer::cmdTypeText" in debug
        and "_bridge.sendKeys(chunk, chunkLen)" in type_text
        and "delay(50);" not in type_text
        and "CMD_SEND_KEY" in bridge
        and "CMD_WRITE_KEYS" in bridge.split("bool FpgaBridge::sendKeys", 1)[1].split("bool FpgaBridge::readScreen", 1)[0]
        and "writeBytesBulk(header, sizeof(header), data + off, chunk)" in bridge
        and "writeBytes(cmd, sizeof(cmd))" in bridge,
        "unit tests cover boot order": "FindAutoboot_PrefersInsertedFloppyOverHardDrive" in unit_dm
        and "SelectBootDevice_PrefersInsertedFloppyWhenNoAutobootExists" in unit_dm,
        "unit tests cover runtime load command": "LoadRuntime_LoadsExact16KImageIntoPrimaryRuntime" in unit_fio,
        "unit tests cover RNG command": "RngCommand_ReturnsProviderBytes" in unit_fio,
        "unit tests cover NVG load command": "NvgLoad_DecodesSparseNvgIntoGraphicsPlane" in unit_fio,
        "unit tests cover primary ROM swap alias": "WriteRomSwapPrimary_SelectsPrimaryRuntimeRom" in unit_rom,
        "NovaZ example builds launcher plus runtime": "AUTOBOOT := $(BUILD_DIR)/AUTOBOOT.bin" in novaz_make
        and "RUNTIME := $(BUILD_DIR)/novaz.bin" in novaz_make
        and "IMAGE ?= $(DIST_DIR)/fd0.ndi" in novaz_make
        and "--runtime" in novaz_make,
        "NovaZ launcher replaces BASIC runtime": "fio_load_runtime" in novaz_auto
        and "STA REG_ROMSWAP" in novaz_auto
        and "JMP ($FFFC)" in novaz_auto,
        "NovaZ runtime requests XRAM workspaces through the MEMORY service": "JSR zstory_load_default" in novaz_runtime
        and "JMP xram_xload" in novaz_zstory
        and "JSR zstory_init_xram_workspaces" in novaz_zstory
        and "MODULE_ID_MEMORY" in novaz_zstory
        and "MEM_ALLOC" in novaz_zstory
        and "MEM_RESET_USAGE" in novaz_zstory
        and "ZSTORY_AUX_ALLOC_HI     = $FF" in novaz_zstory
        and "XRAM_NOVAZ_" not in novaz_zstory
        and "zstory_dynamic_base_l" in novaz_zstory
        and "zstory_cache_base_l" in novaz_zstory
        and "ZSTORY_AUX_PICS_INDEX_OFF_M" in novaz_zstory_inc
        and "ZSTORY_AUX_PICS_BOUNCE_OFF_M" in novaz_zstory_inc
        and "ZSTORY_AUX_PICS_INDEX_OFF_M   = $64" in novaz_zstory_inc
        and "ZSTORY_AUX_PICS_BOUNCE_OFF_M  = $84" in novaz_zstory_inc
        and "ZSTORY_AUX_SAVE_CHAR_OFF_M" in novaz_zstory_inc
        and "ZSTORY_AUX_SAVE_GFX_OFF_M" in novaz_zstory_inc
        and "SYS_NUI_SAVE_UNDER_FULL" in novaz_save_load
        and "SYS_NUI_RESTORE_UNDER_FULL" in novaz_save_load
        and "save_load_copy_header_desc_to_row" in novaz_save_load
        and "save_load_title_save" in novaz_save_load
        and "save_load_title_restore" in novaz_save_load
        and "SAVE / RESTORE" not in novaz_save_load
        and "SAVE00.NZS       " not in novaz_save_load
        and "zstory_cache_base_l" in novaz_zvm6
        and "ZSTORY_AUX_PICS_INDEX_OFF_M" in novaz_zvm6
        and "ZSTORY_AUX_PICS_BOUNCE_OFF_M - ZSTORY_AUX_PICS_INDEX_OFF_M" in novaz_zvm6
        and "NZ6_PICS_ALLOC" not in novaz_zvm6
        and "MEM_ALLOC" not in novaz_zvm6
        and "LIB_LOADER_BAND" not in novaz_zvm6
        and '.byte "story.bin"' in novaz_zstory,
    }
    for name, ok in checks.items():
        check(name, ok)


def test_boot_splash_handoff_contract() -> None:
    novahost = read("e6502.ESP32/novahost/novahost.ino")
    splash = novahost.split("bool showBootSplash()", 1)[1]
    splash = splash.split("// =========================================================================\n// WiFi setup", 1)[0]
    rom_load = novahost.split("bool loadRomsToFPGA()", 1)[1]
    rom_load = rom_load.split("// =========================================================================\n// Accept new log viewer", 1)[0]
    fade_out = splash.find("fadeBootSplash(15, 0, (uint16_t)cfg.fadeOutMs);")
    clear_text = splash.find("clearVgcText()", fade_out)
    restore_text = splash.find("restoreBootSplashVideoState();", clear_text)
    reset_hold = rom_load.find("fpgaBridge.resetHold()")
    rom_clear_text = rom_load.find("clearVgcText()", reset_hold)
    stream_basic = rom_load.find("streamRomAsset(0", rom_clear_text)

    checks = {
        "NovaHost defines text VRAM dimensions": "VGC_TEXT_LEN       = 80UL * 50UL" in novahost,
        "NovaHost can fill text chars from ESP": "VGC_SPACE_CHAR" in novahost
        and "fillVgcRange(VGC_SPACE_CHAR" in novahost
        and "0x20, \"chars\"" in novahost,
        "NovaHost can reset text colors from ESP": "VGC_SPACE_COLOR" in novahost
        and "fillVgcRange(VGC_SPACE_COLOR" in novahost
        and "0x0F, \"colors\"" in novahost,
        "NovaHost clears text attributes from ESP": "VGC_SPACE_TEXTATTR" in novahost
        and "fillVgcRange(VGC_SPACE_TEXTATTR" in novahost
        and "0x00, \"attrs\"" in novahost,
        "boot splash uploads an embedded palette through the active palette port": "VGC_PALETTE_INDEX" in novahost
        and "VGC_PALETTE_DATA" in novahost
        and "VGC_PALETTE_MODE" not in novahost
        and "VGC_PALMODE_CUSTOM" not in novahost,
        "boot splash is driven by boot.json config with fallback asset": "BootSplashConfig readBootSplashConfig()" in novahost
        and "parseBootSplashConfigText(buf, len, cfg)" in novahost
        and "cfg.assetPath" in splash
        and "BOOT_LOGO_NVG_PATHS[0]" in splash
        and "Boot splash skipped: disabled in boot.json" in splash,
        "boot splash config controls colors and timing": "setBootSplashVideoState(0, (uint8_t)cfg.background" in splash
        and "(uint8_t)cfg.border" in splash
        and "fadeBootSplash(0, 15, (uint16_t)cfg.fadeInMs);" in splash
        and "delay(cfg.holdMs);" in splash
        and "fadeBootSplash(15, 0, (uint16_t)cfg.fadeOutMs);" in splash,
        "NovaHost resets active palette to C64 for boot handoff": "VGC_DEFAULT_C64_PALETTE" in novahost
        and "bool resetVgcDefaultPalette()" in novahost
        and "fpgaBridge.poke(VGC_PALETTE_INDEX, 0x00)" in novahost
        and "fpgaBridge.poke(VGC_PALETTE_DATA, VGC_DEFAULT_C64_PALETTE[i])" in novahost
        and "resetVgcDefaultPalette()" in novahost.split("bool clearVgcText()", 1)[1].split("bool readByte", 1)[0]
        and "resetVgcDefaultPalette()" in novahost.split("bool setBootSplashVideoState", 1)[1].split("void restoreBootSplashVideoState", 1)[0]
        and "resetVgcDefaultPalette();" in novahost.split("void restoreBootSplashVideoState", 1)[1].split("void fadeBootSplash", 1)[0],
        "boot splash clears text before restoring text mode": fade_out >= 0
        and clear_text > fade_out
        and restore_text > clear_text,
        "ROM load clears text after reset hold before streaming": reset_hold >= 0
        and rom_clear_text > reset_hold
        and stream_basic > rom_clear_text,
    }
    for name, ok in checks.items():
        check(name, ok)


def main() -> int:
    tests = [
        ("serial commands", test_serial_commands),
        ("management routes", test_management_routes),
        ("password redaction", test_password_redaction),
        ("host-status LED contract", test_host_status_led_contract),
        ("FPGA SPI bridge contract", test_fpga_spi_bridge_contract),
        ("FIO clear-error contract", test_fio_clear_error_contract),
        ("FIO SD dispatch contract", test_fio_sd_dispatch_contract),
        ("NIC command sequence contract", test_nic_command_sequence_contract),
        ("large NDI BAM contract", test_large_ndi_bam_contract),
        ("runtime autoboot contract", test_runtime_autoboot_contract),
        ("boot splash handoff contract", test_boot_splash_handoff_contract),
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
