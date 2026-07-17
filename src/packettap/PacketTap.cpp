#include "PacketTap.h"
#include <string.h>
PacketTap& PacketTap::instance(){static PacketTap t;return t;}
void PacketTap::begin(){}
void PacketTap::loop(){}
bool PacketTap::capture(const uint8_t* payload,uint16_t length,int16_t rssi,int16_t snr_x10,uint32_t timestamp_ms){
 if(length>MAX_TRANS_UNIT) length=MAX_TRANS_UNIT;
 PacketFrame f;
 f.timestamp_ms=timestamp_ms;
 f.rssi_dbm=rssi;
 f.snr_x10=snr_x10;
 f.payload_length=length;
 if(payload&&length) memcpy(f.payload,payload,length);
 return queue_.push(f);
}
