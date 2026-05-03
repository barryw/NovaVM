// NovaWifiManager -- shared WiFi configuration/control for REST and serial.

#ifndef NOVAHOST_NOVA_WIFI_H
#define NOVAHOST_NOVA_WIFI_H

#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>

class NovaWifiManager {
public:
    struct Config {
        String ssid;
        String password;
        bool useStatic = false;
        String staticIp;
        String gateway;
        String subnet;
        String dns;
        bool stored = false;
    };

    struct Status {
        bool configured = false;
        bool connected = false;
        bool wantConnected = false;
        bool passwordSet = false;
        bool useStatic = false;
        String ssid;
        String staticIp;
        String gateway;
        String subnet;
        String dns;
        String localIp;
        String localGateway;
        String localSubnet;
        String localDns;
        String mac;
        int32_t rssi = 0;
        wl_status_t wifiStatus = WL_IDLE_STATUS;
        String lastError;
    };

    void begin(const Config& defaults);

    Config config() const;
    Status status() const;

    bool configured() const;
    bool connected() const;
    bool wantConnected() const { return _wantConnected; }

    bool saveConfig(const Config& cfg, String* error = nullptr);
    void forget();

    bool connect(uint32_t timeoutMs = 10000);
    bool reconnect(uint32_t timeoutMs = 10000);
    void disconnect();
    void loop();

    static const char* statusName(wl_status_t status);

private:
    Preferences _prefs;
    Config _config;
    bool _wantConnected = false;
    unsigned long _lastReconnectAttempt = 0;
    String _lastError;

    static bool configLooksConfigured(const Config& cfg);
    static bool parseIp(const String& text, IPAddress& out);
    static bool validateStaticConfig(const Config& cfg, String* error);

    bool applyIpConfig(String* error);
    void loadStoredOrDefault(const Config& defaults);
    void persist();
};

#endif
