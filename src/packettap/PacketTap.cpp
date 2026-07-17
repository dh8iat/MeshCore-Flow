#include "PacketTap.h"
#include <string.h>
#include "outputs/TcpOutput.h"

PacketTap& PacketTap::instance() {
    static PacketTap tap;
    return tap;
}

void PacketTap::begin() {
    TcpOutput::instance().begin();
}

void PacketTap::loop() {
    TcpOutput& output = TcpOutput::instance();
    output.loop();

    if (!output.ready()) return;

    PacketFrame frame;
    while (queue_.pop(frame)) {
        if (!output.send(frame)) break;
    }
}

bool PacketTap::capture(const uint8_t* payload,
                        uint16_t length,
                        int16_t rssi,
                        int16_t snr_x10,
                        uint32_t timestamp_ms) {
    if (length > MAX_TRANS_UNIT) length = MAX_TRANS_UNIT;

    PacketFrame frame;
    frame.timestamp_ms = timestamp_ms;
    frame.rssi_dbm = rssi;
    frame.snr_x10 = snr_x10;
    frame.payload_length = length;

    if (payload != nullptr && length > 0)
        memcpy(frame.payload, payload, length);

    return queue_.push(frame);
}
