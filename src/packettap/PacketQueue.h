#pragma once
#include "PacketFrame.h"
class PacketQueue{
public:
    static constexpr uint8_t Capacity=32;
    bool push(const PacketFrame&);
    bool pop(PacketFrame&);
    bool empty() const;
    bool full() const;
    uint8_t size() const;
private:
    PacketFrame queue_[Capacity];
    uint8_t head_=0,tail_=0,count_=0;
};
