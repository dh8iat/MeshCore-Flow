#include "TcpOutput.h"

namespace {

void putU16LE(uint8_t* p, uint16_t value) {
    p[0] = static_cast<uint8_t>(value);
    p[1] = static_cast<uint8_t>(value >> 8);
}

void putI16LE(uint8_t* p, int16_t value) {
    putU16LE(p, static_cast<uint16_t>(value));
}

void putU32LE(uint8_t* p, uint32_t value) {
    p[0] = static_cast<uint8_t>(value);
    p[1] = static_cast<uint8_t>(value >> 8);
    p[2] = static_cast<uint8_t>(value >> 16);
    p[3] = static_cast<uint8_t>(value >> 24);
}

}  // namespace

TcpOutput& TcpOutput::instance() {
    static TcpOutput output;
    return output;
}

void TcpOutput::begin() {
    // PacketTap starts disabled. Do not touch Wi-Fi or TCP during boot.
}

void TcpOutput::setEnabled(bool enabled) {
    if (enabled_ == enabled) {
        return;
    }

    enabled_ = enabled;

    if (!enabled_) {
        // Close only PacketTap's TCP socket. Wi-Fi may be shared with OTA or Companion.
        disconnectTcp();
        return;
    }

    nextWifiAttemptMs_ = 0;
    nextTcpAttemptMs_ = 0;
}

bool TcpOutput::isEnabled() const {
    return enabled_;
}

void TcpOutput::loop() {
    if (!enabled_) {
        return;
    }

    if (WiFi.status() != WL_CONNECTED) {
        disconnectTcp();

        const uint32_t now = millis();

        if (static_cast<int32_t>(now - nextWifiAttemptMs_) >= 0) {
            connectWifi();
        }

        return;
    }

    if (!client_.connected()) {
        const uint32_t now = millis();

        if (static_cast<int32_t>(now - nextTcpAttemptMs_) >= 0) {
            connectTcp();
        }
    }
}

bool TcpOutput::ready() {
    return enabled_ && WiFi.status() == WL_CONNECTED && client_.connected();
}

bool TcpOutput::send(const PacketFrame& frame) {
    if (!ready()) {
        return false;
    }

    /*
     * PacketTap Protocol v1
     *
     * Offset  Size  Field
     * 0       4     Magic
     * 4       1     Version
     * 5       1     Flags
     * 6       2     Payload length
     * 8       4     Timestamp
     * 12      2     RSSI
     * 14      2     SNR x10
     * 16      n     Payload
     * 16+n    4     CRC32
     */

    constexpr size_t HeaderSize = 16;
    constexpr size_t CrcSize = 4;
    constexpr uint8_t Flags = 0;

    if (frame.payload_length > MAX_TRANS_UNIT) {
        return false;
    }

    uint8_t buffer[HeaderSize + MAX_TRANS_UNIT + CrcSize];

    buffer[0] = 'P';
    buffer[1] = 'K';
    buffer[2] = 'T';
    buffer[3] = 'P';
    buffer[4] = ProtocolVersion;
    buffer[5] = Flags;

    putU16LE(buffer + 6, frame.payload_length);
    putU32LE(buffer + 8, frame.timestamp_ms);
    putI16LE(buffer + 12, frame.rssi_dbm);
    putI16LE(buffer + 14, frame.snr_x10);

    for (uint16_t i = 0; i < frame.payload_length; ++i) {
        buffer[HeaderSize + i] = frame.payload[i];
    }

    const size_t bodyLength = HeaderSize + frame.payload_length;

    putU32LE(
        buffer + bodyLength,
        crc32(buffer, bodyLength)
    );

    if (!writeAll(buffer, bodyLength + CrcSize)) {
        disconnectTcp();
        nextTcpAttemptMs_ = millis() + TcpRetryMs;
        return false;
    }

    return true;
}

void TcpOutput::connectWifi() {
    nextWifiAttemptMs_ = millis() + WifiRetryMs;

    if (!enabled_ || PACKETTAP_WIFI_SSID[0] == '\0') {
        return;
    }

    if (WiFi.status() == WL_CONNECTED) {
        return;
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(
        PACKETTAP_WIFI_SSID,
        PACKETTAP_WIFI_PASSWORD
    );
}

void TcpOutput::connectTcp() {
    nextTcpAttemptMs_ = millis() + TcpRetryMs;

    if (!enabled_ || PACKETTAP_TCP_HOST[0] == '\0') {
        return;
    }

    disconnectTcp();

    client_.setNoDelay(true);

    client_.connect(
        PACKETTAP_TCP_HOST,
        PACKETTAP_TCP_PORT
    );
}

void TcpOutput::disconnectTcp() {
    client_.stop();
}

bool TcpOutput::writeAll(
    const uint8_t* data,
    size_t length
) {
    if (!enabled_ || !client_.connected()) {
        return false;
    }

    const size_t written = client_.write(data, length);

    return written == length;
}

uint32_t TcpOutput::crc32(
    const uint8_t* data,
    size_t length
) {
    uint32_t crc = 0xFFFFFFFFUL;

    for (size_t i = 0; i < length; ++i) {
        crc ^= data[i];

        for (uint8_t bit = 0; bit < 8; ++bit) {
            crc =
                (crc >> 1) ^
                (
                    0xEDB88320UL &
                    (0U - (crc & 1U))
                );
        }
    }

    return ~crc;
}
