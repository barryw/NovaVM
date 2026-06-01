// ManagementServer -- stateful TCP control protocol for nova webserver.
//
// Browser HTTP stays on the workstation. NovaHost exposes this compact
// board-facing service on port 6504 using fixed frames plus CBOR payloads.

#ifndef NOVAHOST_MANAGEMENT_SERVER_H
#define NOVAHOST_MANAGEMENT_SERVER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SD.h>
#include <WiFi.h>
#include <vector>
#include "device_manager.h"
#include "nova_wifi.h"

class ManagementServer {
public:
    ManagementServer(DeviceManager& dm, NovaWifiManager& wifi,
                     uint16_t port = 6504)
        : _dm(dm), _wifi(wifi), _server(port) {}

    void begin();
    void loop();

private:
    static constexpr uint16_t PROTOCOL_VERSION = 1;
    static constexpr size_t FRAME_HEADER_BYTES = 24;
    static constexpr uint32_t MAX_CBOR_BYTES = 32768;
    static constexpr uint32_t MAX_RAW_BYTES = 16384;

    enum Command : uint16_t {
        CMD_HELLO = 1,
        CMD_GET_STATUS = 2,
        CMD_LIST_DIRECTORY = 3,
        CMD_READ_FILE = 4,
        CMD_WRITE_FILE_BEGIN = 5,
        CMD_WRITE_FILE_CHUNK = 6,
        CMD_WRITE_FILE_COMMIT = 7,
        CMD_WRITE_FILE_ABORT = 8,
        CMD_DELETE_PATH = 9,
        CMD_MOUNT_DRIVE = 10,
        CMD_UNMOUNT_DRIVE = 11,
        CMD_SET_RUNTIME_CONFIG = 12,
        CMD_AUDIO_STOP = 13,
        CMD_VM_RESET = 14,
        CMD_HOST_REBOOT = 15,
        CMD_WIFI_SCAN = 16,
        CMD_WIFI_CONFIG = 17,
        CMD_WIFI_ACTION = 18,
        CMD_READ_FILE_CHUNK = 19
    };

    struct Frame {
        uint16_t command = 0;
        uint16_t flags = 0;
        uint32_t request_id = 0;
        uint32_t payload_len = 0;
        uint32_t raw_len = 0;
    };

    DeviceManager& _dm;
    NovaWifiManager& _wifi;
    WiFiServer _server;
    WiFiClient _client;
    bool _started = false;

    bool _upload_active = false;
    bool _upload_locked = false;
    File _upload_file;
    char _upload_path[300] = {0};
    char _upload_tmp_path[320] = {0};
    uint32_t _upload_expected = 0;
    uint32_t _upload_written = 0;

    void close_client();
    void cleanup_upload();
    void handle_frame(const Frame& frame, const uint8_t* payload,
                      size_t payload_len, const uint8_t* raw,
                      size_t raw_len);

    bool read_exact(uint8_t* out, size_t len, uint32_t timeout_ms);
    bool read_frame_header(Frame& frame);
    bool write_all(const uint8_t* data, size_t len);
    void send_frame(uint16_t command, uint32_t request_id,
                    const std::vector<uint8_t>& payload,
                    const uint8_t* raw = nullptr, size_t raw_len = 0);
    void send_error(uint16_t command, uint32_t request_id, const char* code,
                    const char* message);

    void handle_hello(uint32_t request_id);
    void handle_get_status(uint32_t request_id);
    void handle_list_directory(uint16_t command, uint32_t request_id,
                               const uint8_t* payload, size_t payload_len);
    void handle_read_file(uint16_t command, uint32_t request_id,
                          const uint8_t* payload, size_t payload_len);
    void handle_read_file_chunk(uint16_t command, uint32_t request_id,
                                const uint8_t* payload, size_t payload_len);
    void handle_write_begin(uint16_t command, uint32_t request_id,
                            const uint8_t* payload, size_t payload_len);
    void handle_write_chunk(uint16_t command, uint32_t request_id,
                            const uint8_t* payload, size_t payload_len,
                            const uint8_t* raw, size_t raw_len);
    void handle_write_commit(uint16_t command, uint32_t request_id);
    void handle_write_abort(uint16_t command, uint32_t request_id);
    void handle_delete_path(uint16_t command, uint32_t request_id,
                            const uint8_t* payload, size_t payload_len);
    void handle_mount_drive(uint16_t command, uint32_t request_id,
                            const uint8_t* payload, size_t payload_len);
    void handle_unmount_drive(uint16_t command, uint32_t request_id,
                              const uint8_t* payload, size_t payload_len);
    void handle_runtime_config(uint16_t command, uint32_t request_id,
                               const uint8_t* payload, size_t payload_len);
    void handle_audio_stop(uint16_t command, uint32_t request_id);
    void handle_vm_reset(uint16_t command, uint32_t request_id);
    void handle_host_reboot(uint16_t command, uint32_t request_id);
    void handle_wifi_scan(uint16_t command, uint32_t request_id);
    void handle_wifi_config(uint16_t command, uint32_t request_id,
                            const uint8_t* payload, size_t payload_len);
    void handle_wifi_action(uint16_t command, uint32_t request_id,
                            const uint8_t* payload, size_t payload_len);

    bool path_sane(const char* path, bool allow_empty);
    bool path_safe_for_write(const char* path);
    bool ensure_parent_dirs(const char* full_path);
    bool persist_drive_mount_config(const char* prefix, const char* sd_path);
    bool load_drive_mount_config(const char* prefix, char* out, size_t out_len);
    bool write_boot_config(JsonDocument& doc);
    bool read_boot_config_text(String& out);
    size_t sd_file_size(const char* path);
    bool verify_sd_file_size(const char* path, uint32_t expected,
                             char* error, size_t error_len);
};

#endif
