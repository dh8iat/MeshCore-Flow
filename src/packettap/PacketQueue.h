#pragma once

#include "PacketFrame.h"

class PacketTapQueue
{
public:
    static constexpr uint8_t Capacity = 32;

    bool push(const PacketFrame& frame);
    bool pop(PacketFrame& frame);

    bool empty() const;
    bool full() const;
    uint8_t size() const;

private:
    PacketFrame queue_[Capacity];
    uint8_t head_ = 0;
    uint8_t tail_ = 0;
    uint8_t count_ = 0;
};
