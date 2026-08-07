#include "PacketTap.h"

#include <string.h>

#include "outputs/TcpOutput.h"

PacketTap& PacketTap::instance() {
    static PacketTap tap;
    return tap;
}

void PacketTap::begin() {
    // PacketTap intentionally starts disabled after every reboot.
    TcpOutput::instance().begin();
}

void PacketTap::loop() {
    if (!enabled_) {
        return;
    }

    TcpOutput& output = TcpOutput::instance();
    output.loop();

    if (!output.ready()) {
        return;
    }

    PacketFrame frame;
    while (queue_.pop(frame)) {
        if (!output.send(frame)) {
            break;
        }
    }
}

void PacketTap::setEnabled(bool enabled) {
    if (enabled_ == enabled) {
        return;
    }

    enabled_ = enabled;

    if (!enabled_) {
        PacketFrame frame;
        while (queue_.pop(frame)) {
            // Discard queued captures when PacketTap is switched off.
        }
    }

    TcpOutput::instance().setEnabled(enabled_);
}

bool PacketTap::isEnabled() const {
    return enabled_;
}

bool PacketTap::capture(
    const uint8_t* payload,
    uint16_t length,
    int16_t rssi,
    int16_t snr_x10,
    uint32_t timestamp_ms
) {
    // This is the hot RX path. Return before copying or queueing anything.
    if (!enabled_) {
        return true;
    }

    if (length > MAX_TRANS_UNIT) {
        length = MAX_TRANS_UNIT;
    }

    PacketFrame frame;
    frame.timestamp_ms = timestamp_ms;
    frame.rssi_dbm = rssi;
    frame.snr_x10 = snr_x10;
    frame.payload_length = length;

    if (payload != nullptr && length > 0) {
        memcpy(frame.payload, payload, length);
    }

    return queue_.push(frame);
}
