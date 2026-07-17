#include "TcpOutput.h"

namespace {
void putU16LE(uint8_t* p, uint16_t v) {
    p[0] = static_cast<uint8_t>(v);
    p[1] = static_cast<uint8_t>(v >> 8);
}
void putU32LE(uint8_t* p, uint32_t v) {
    p[0] = static_cast<uint8_t>(v);
    p[1] = static_cast<uint8_t>(v >> 8);
    p[2] = static_cast<uint8_t>(v >> 16);
    p[3] = static_cast<uint8_t>(v >> 24);
}
}

TcpOutput& TcpOutput::instance() {
    static TcpOutput output;
    return output;
}

void TcpOutput::begin() {
    WiFi.mode(WIFI_STA);
    connectWifi();
}

void TcpOutput::loop() {
    if (WiFi.status() != WL_CONNECTED) {
        disconnectTcp();
        const uint32_t now = millis();
        if (static_cast<int32_t>(now - nextWifiAttemptMs_) >= 0)
            connectWifi();
        return;
    }

    if (!client_.connected()) {
        const uint32_t now = millis();
        if (static_cast<int32_t>(now - nextTcpAttemptMs_) >= 0)
            connectTcp();
    }
}

bool TcpOutput::ready() const {
    return WiFi.status() == WL_CONNECTED && client_.connected();
}

bool TcpOutput::send(const PacketFrame& frame) {
    if (!ready()) return false;

    constexpr size_t HeaderSize = 15;
    constexpr size_t CrcSize = 4;
    uint8_t buffer[HeaderSize + MAX_TRANS_UNIT + CrcSize];

    putU32LE(buffer + 0, Magic);
    buffer[4] = ProtocolVersion;
    putU32LE(buffer + 5, frame.timestamp_ms);
    putU16LE(buffer + 9, static_cast<uint16_t>(frame.rssi_dbm));
    putU16LE(buffer + 11, static_cast<uint16_t>(frame.snr_x10));
    putU16LE(buffer + 13, frame.payload_length);

    for (uint16_t i = 0; i < frame.payload_length; ++i)
        buffer[HeaderSize + i] = frame.payload[i];

    const size_t bodyLength = HeaderSize + frame.payload_length;
    putU32LE(buffer + bodyLength, crc32(buffer, bodyLength));

    if (!writeAll(buffer, bodyLength + CrcSize)) {
        disconnectTcp();
        nextTcpAttemptMs_ = millis() + TcpRetryMs;
        return false;
    }
    return true;
}

void TcpOutput::connectWifi() {
    nextWifiAttemptMs_ = millis() + WifiRetryMs;
    if (PACKETTAP_WIFI_SSID[0] == '\0') return;
    WiFi.disconnect(false, false);
    WiFi.begin(PACKETTAP_WIFI_SSID, PACKETTAP_WIFI_PASSWORD);
}

void TcpOutput::connectTcp() {
    nextTcpAttemptMs_ = millis() + TcpRetryMs;
    if (PACKETTAP_TCP_HOST[0] == '\0') return;
    disconnectTcp();
    client_.setNoDelay(true);
    client_.connect(PACKETTAP_TCP_HOST, PACKETTAP_TCP_PORT);
}

void TcpOutput::disconnectTcp() {
    client_.stop();
}

bool TcpOutput::writeAll(const uint8_t* data, size_t length) {
    size_t written = 0;
    while (written < length) {
        const size_t n = client_.write(data + written, length - written);
        if (n == 0) return false;
        written += n;
    }
    return true;
}

uint32_t TcpOutput::crc32(const uint8_t* data, size_t length) {
    uint32_t crc = 0xFFFFFFFFUL;
    for (size_t i = 0; i < length; ++i) {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; ++bit)
            crc = (crc >> 1) ^ (0xEDB88320UL & (0U - (crc & 1U)));
    }
    return ~crc;
}
