#include "nova_wifi.h"

void NovaWifiManager::begin(const Config& defaults) {
    _prefs.begin("novawifi", false);
    loadStoredOrDefault(defaults);
    _wantConnected = configured();
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(false);
}

NovaWifiManager::Config NovaWifiManager::config() const {
    return _config;
}

NovaWifiManager::Status NovaWifiManager::status() const {
    Status st;
    st.configured = configured();
    st.connected = connected();
    st.wantConnected = _wantConnected;
    st.passwordSet = _config.password.length() > 0;
    st.useStatic = _config.useStatic;
    st.ssid = _config.ssid;
    st.staticIp = _config.staticIp;
    st.gateway = _config.gateway;
    st.subnet = _config.subnet;
    st.dns = _config.dns;
    st.localIp = WiFi.localIP().toString();
    st.localGateway = WiFi.gatewayIP().toString();
    st.localSubnet = WiFi.subnetMask().toString();
    st.localDns = WiFi.dnsIP().toString();
    st.mac = WiFi.macAddress();
    st.rssi = connected() ? WiFi.RSSI() : 0;
    st.wifiStatus = WiFi.status();
    st.lastError = _lastError;
    return st;
}

bool NovaWifiManager::configured() const {
    return configLooksConfigured(_config);
}

bool NovaWifiManager::connected() const {
    return WiFi.status() == WL_CONNECTED;
}

bool NovaWifiManager::saveConfig(const Config& cfg, String* error) {
    if (cfg.useStatic && !validateStaticConfig(cfg, error))
        return false;

    _config = cfg;
    _config.stored = _config.ssid.length() > 0;
    persist();

    if (!configured())
        _wantConnected = false;

    _lastError = "";
    return true;
}

void NovaWifiManager::forget() {
    disconnect();
    _prefs.clear();
    _config = Config();
    _lastError = "";
}

bool NovaWifiManager::connect(uint32_t timeoutMs) {
    if (!configured()) {
        _wantConnected = false;
        _lastError = "wifi not configured";
        return false;
    }

    _wantConnected = true;

    String error;
    if (!applyIpConfig(&error)) {
        _lastError = error;
        return false;
    }

    WiFi.mode(WIFI_STA);
    WiFi.disconnect(false);
    delay(100);
    WiFi.begin(_config.ssid.c_str(), _config.password.c_str());

    unsigned long deadline = millis() + timeoutMs;
    while (WiFi.status() != WL_CONNECTED && millis() < deadline) {
        delay(250);
        yield();
    }

    if (connected()) {
        _lastError = "";
        return true;
    }

    _lastError = String("connect failed: ") + statusName(WiFi.status());
    return false;
}

bool NovaWifiManager::reconnect(uint32_t timeoutMs) {
    WiFi.disconnect(false);
    delay(200);
    return connect(timeoutMs);
}

void NovaWifiManager::disconnect() {
    _wantConnected = false;
    WiFi.disconnect(false);
}

void NovaWifiManager::loop() {
    if (!_wantConnected || !configured() || connected())
        return;

    unsigned long now = millis();
    if (now - _lastReconnectAttempt < 10000)
        return;

    _lastReconnectAttempt = now;
    WiFi.reconnect();
}

const char* NovaWifiManager::statusName(wl_status_t status) {
    switch (status) {
        case WL_NO_SHIELD:       return "no_shield";
        case WL_IDLE_STATUS:     return "idle";
        case WL_NO_SSID_AVAIL:   return "no_ssid";
        case WL_SCAN_COMPLETED:  return "scan_completed";
        case WL_CONNECTED:       return "connected";
        case WL_CONNECT_FAILED:  return "connect_failed";
        case WL_CONNECTION_LOST: return "connection_lost";
        case WL_DISCONNECTED:    return "disconnected";
        default:                 return "unknown";
    }
}

bool NovaWifiManager::configLooksConfigured(const Config& cfg) {
    return cfg.ssid.length() > 0 && cfg.ssid != "your_ssid";
}

bool NovaWifiManager::parseIp(const String& text, IPAddress& out) {
    return text.length() > 0 && out.fromString(text);
}

bool NovaWifiManager::validateStaticConfig(const Config& cfg, String* error) {
    IPAddress ip, gateway, subnet, dns;
    if (!parseIp(cfg.staticIp, ip)) {
        if (error) *error = "invalid staticIp";
        return false;
    }
    if (!parseIp(cfg.gateway, gateway)) {
        if (error) *error = "invalid gateway";
        return false;
    }
    if (!parseIp(cfg.subnet, subnet)) {
        if (error) *error = "invalid subnet";
        return false;
    }
    if (!parseIp(cfg.dns, dns)) {
        if (error) *error = "invalid dns";
        return false;
    }
    return true;
}

bool NovaWifiManager::applyIpConfig(String* error) {
    if (!_config.useStatic) {
        IPAddress none(0, 0, 0, 0);
        WiFi.config(none, none, none);
        return true;
    }

    if (!validateStaticConfig(_config, error))
        return false;

    IPAddress ip, gateway, subnet, dns;
    ip.fromString(_config.staticIp);
    gateway.fromString(_config.gateway);
    subnet.fromString(_config.subnet);
    dns.fromString(_config.dns);
    if (!WiFi.config(ip, gateway, subnet, dns)) {
        if (error) *error = "WiFi.config failed";
        return false;
    }
    return true;
}

void NovaWifiManager::loadStoredOrDefault(const Config& defaults) {
    String ssid = _prefs.getString("ssid", "");
    if (ssid.length() > 0) {
        _config.ssid = ssid;
        _config.password = _prefs.getString("password", "");
        _config.useStatic = _prefs.getBool("useStatic", false);
        _config.staticIp = _prefs.getString("staticIp", "");
        _config.gateway = _prefs.getString("gateway", "");
        _config.subnet = _prefs.getString("subnet", "");
        _config.dns = _prefs.getString("dns", "");
        _config.stored = true;
        return;
    }

    _config = defaults;
    _config.stored = false;
}

void NovaWifiManager::persist() {
    _prefs.putString("ssid", _config.ssid);
    _prefs.putString("password", _config.password);
    _prefs.putBool("useStatic", _config.useStatic);
    _prefs.putString("staticIp", _config.staticIp);
    _prefs.putString("gateway", _config.gateway);
    _prefs.putString("subnet", _config.subnet);
    _prefs.putString("dns", _config.dns);
}
