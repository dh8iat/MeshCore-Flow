#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include "../PacketFrame.h"

#ifndef PACKETTAP_WIFI_SSID
#define PACKETTAP_WIFI_SSID ""
#endif
#ifndef PACKETTAP_WIFI_PASSWORD
#define PACKETTAP_WIFI_PASSWORD ""
#endif
#ifndef PACKETTAP_TCP_HOST
#define PACKETTAP_TCP_HOST ""
#endif
#ifndef PACKETTAP_TCP_PORT
#define PACKETTAP_TCP_PORT 9000
#endif

class TcpOutput {
public:
    static TcpOutput& instance();
    void begin();
    void loop();
    bool ready();
    bool send(const PacketFrame& frame);

private:
    TcpOutput() = default;
    void connectWifi();
    void connectTcp();
    void disconnectTcp();
    bool writeAll(const uint8_t* data, size_t length);
    static uint32_t crc32(const uint8_t* data, size_t length);

    WiFiClient client_;
    uint32_t nextWifiAttemptMs_ = 0;
    uint32_t nextTcpAttemptMs_ = 0;

    static constexpr uint32_t WifiRetryMs = 10000;
    static constexpr uint32_t TcpRetryMs = 5000;
    static constexpr uint32_t Magic = 0x504B5450UL;
    static constexpr uint8_t ProtocolVersion = 1;
};
