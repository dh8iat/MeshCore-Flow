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

char hexDigit(uint8_t value) {
    value &= 0x0F;
    return value < 10
        ? static_cast<char>('0' + value)
        : static_cast<char>('a' + value - 10);
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

void TcpOutput::setReceiverIdentity(
    const uint8_t* public_key,
    size_t public_key_length,
    const char* node_name,
    const char* model,
    const char* firmware,
    const char* build,
    const char* node_role
) {
    bytesToHex(
        public_key_,
        sizeof(public_key_),
        public_key,
        public_key_length
    );
    copyText(node_name_, sizeof(node_name_), node_name);
    copyText(model_, sizeof(model_), model);
    copyText(firmware_, sizeof(firmware_), firmware);
    copyText(build_, sizeof(build_), build);
    copyText(node_role_, sizeof(node_role_), node_role);
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

    if (client_.connect(
        PACKETTAP_TCP_HOST,
        PACKETTAP_TCP_PORT
    )) {
        if (!sendHello()) {
            disconnectTcp();
            nextTcpAttemptMs_ = millis() + TcpRetryMs;
        }
    }
}

void TcpOutput::disconnectTcp() {
    client_.stop();
}

bool TcpOutput::sendHello() {
    /*
     * PacketTap HELLO Protocol v2
     *
     * Header:
     *   0..3  "PKTH"
     *   4     protocol version (2)
     *   5     flags
     *   6..7  payload length, little endian
     *
     * Payload:
     *   6 one-byte string lengths, followed by strings:
     *   public_key, node_name, model, firmware, build, node_role
     *
     * Trailer:
     *   CRC32 over header + payload
     */
    constexpr size_t HeaderSize = 8;
    constexpr size_t FieldCount = 6;
    constexpr size_t CrcSize = 4;
    constexpr uint8_t Flags = 0;

    const char* fields[FieldCount] = {
        public_key_,
        node_name_,
        model_,
        firmware_,
        build_,
        node_role_,
    };

    uint8_t lengths[FieldCount];
    size_t stringsLength = 0;

    for (size_t i = 0; i < FieldCount; ++i) {
        size_t length = strlen(fields[i]);
        if (length > 255) {
            length = 255;
        }
        lengths[i] = static_cast<uint8_t>(length);
        stringsLength += length;
    }

    const size_t payloadLength = FieldCount + stringsLength;
    constexpr size_t MaxHelloSize =
        HeaderSize + FieldCount +
        PublicKeyHexSize + NodeNameSize + ModelSize +
        FirmwareSize + BuildSize + NodeRoleSize + CrcSize;

    uint8_t buffer[MaxHelloSize];

    buffer[0] = 'P';
    buffer[1] = 'K';
    buffer[2] = 'T';
    buffer[3] = 'H';
    buffer[4] = HelloProtocolVersion;
    buffer[5] = Flags;
    putU16LE(buffer + 6, static_cast<uint16_t>(payloadLength));

    size_t offset = HeaderSize;

    for (size_t i = 0; i < FieldCount; ++i) {
        buffer[offset++] = lengths[i];
    }

    for (size_t i = 0; i < FieldCount; ++i) {
        const size_t length = lengths[i];
        memcpy(buffer + offset, fields[i], length);
        offset += length;
    }

    putU32LE(buffer + offset, crc32(buffer, offset));
    offset += CrcSize;

    return writeAll(buffer, offset);
}

void TcpOutput::copyText(
    char* dest,
    size_t dest_size,
    const char* source
) {
    if (dest == nullptr || dest_size == 0) {
        return;
    }

    dest[0] = '\0';

    if (source == nullptr) {
        return;
    }

    strncpy(dest, source, dest_size - 1);
    dest[dest_size - 1] = '\0';
}

void TcpOutput::bytesToHex(
    char* dest,
    size_t dest_size,
    const uint8_t* source,
    size_t source_length
) {
    if (dest == nullptr || dest_size == 0) {
        return;
    }

    dest[0] = '\0';

    if (source == nullptr || source_length == 0) {
        return;
    }

    const size_t max_bytes = (dest_size - 1) / 2;
    if (source_length > max_bytes) {
        source_length = max_bytes;
    }

    for (size_t i = 0; i < source_length; ++i) {
        dest[i * 2] = hexDigit(source[i] >> 4);
        dest[i * 2 + 1] = hexDigit(source[i]);
    }

    dest[source_length * 2] = '\0';
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
