#pragma once

#include "PacketQueue.h"

class PacketTap {
public:
    static PacketTap& instance();

    void begin();
    void loop();

    void setEnabled(bool enabled);
    bool isEnabled() const;

    bool capture(
        const uint8_t* payload,
        uint16_t length,
        int16_t rssi,
        int16_t snr_x10,
        uint32_t timestamp_ms
    );

private:
    PacketTap() = default;

    bool enabled_ = false;
    PacketTapQueue queue_;
};
