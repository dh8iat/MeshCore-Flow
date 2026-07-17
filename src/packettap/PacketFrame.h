#pragma once
#include <stdint.h>
#ifndef MAX_TRANS_UNIT
#define MAX_TRANS_UNIT 255
#endif
struct PacketFrame{
    uint32_t timestamp_ms=0;
    int16_t rssi_dbm=0;
    int16_t snr_x10=0;
    uint16_t payload_length=0;
    uint8_t payload[MAX_TRANS_UNIT]={0};
};
