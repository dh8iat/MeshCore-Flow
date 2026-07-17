#include "PacketQueue.h"

bool PacketTapQueue::push(const PacketFrame& frame)
{
    if (full())
        return false;

    queue_[head_] = frame;
    head_ = (head_ + 1) % Capacity;
    ++count_;

    return true;
}

bool PacketTapQueue::pop(PacketFrame& frame)
{
    if (empty())
        return false;

    frame = queue_[tail_];
    tail_ = (tail_ + 1) % Capacity;
    --count_;

    return true;
}

bool PacketTapQueue::empty() const
{
    return count_ == 0;
}

bool PacketTapQueue::full() const
{
    return count_ == Capacity;
}

uint8_t PacketTapQueue::size() const
{
    return count_;
}