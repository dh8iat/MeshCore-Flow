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

    void setEnabled(bool enabled);
    bool isEnabled() const;

    bool ready();
    bool send(const PacketFrame& frame);

    void setReceiverIdentity(
        const uint8_t* public_key,
        size_t public_key_length,
        const char* node_name,
        const char* model,
        const char* firmware,
        const char* build,
        const char* node_role
    );

private:
    TcpOutput() = default;

    void connectWifi();
    void connectTcp();
    void disconnectTcp();
    bool sendHello();
    bool writeAll(const uint8_t* data, size_t length);

    static void copyText(char* dest, size_t dest_size, const char* source);
    static void bytesToHex(
        char* dest,
        size_t dest_size,
        const uint8_t* source,
        size_t source_length
    );
    static uint32_t crc32(const uint8_t* data, size_t length);

    bool enabled_ = false;
    WiFiClient client_;
    uint32_t nextWifiAttemptMs_ = 0;
    uint32_t nextTcpAttemptMs_ = 0;

    static constexpr size_t PublicKeyHexSize = 129;
    static constexpr size_t NodeNameSize = 64;
    static constexpr size_t ModelSize = 32;
    static constexpr size_t FirmwareSize = 64;
    static constexpr size_t BuildSize = 32;
    static constexpr size_t NodeRoleSize = 32;

    char public_key_[PublicKeyHexSize] = {};
    char node_name_[NodeNameSize] = {};
    char model_[ModelSize] = {};
    char firmware_[FirmwareSize] = {};
    char build_[BuildSize] = {};
    char node_role_[NodeRoleSize] = {};

    static constexpr uint32_t WifiRetryMs = 10000;
    static constexpr uint32_t TcpRetryMs = 5000;
    static constexpr uint8_t ProtocolVersion = 1;
    static constexpr uint8_t HelloProtocolVersion = 2;
};
