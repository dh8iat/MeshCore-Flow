#include "PacketQueue.h"
bool PacketQueue::push(const PacketFrame& f){if(full())return false;queue_[head_]=f;head_=(head_+1)%Capacity;++count_;return true;}
bool PacketQueue::pop(PacketFrame& f){if(empty())return false;f=queue_[tail_];tail_=(tail_+1)%Capacity;--count_;return true;}
bool PacketQueue::empty() const{return count_==0;}
bool PacketQueue::full() const{return count_==Capacity;}
uint8_t PacketQueue::size() const{return count_;}
