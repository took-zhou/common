
#ifndef _MTP_MARKET_STRUCT_TYPE_H_
#define _MTP_MARKET_STRUCT_TYPE_H_

#include <cstdint>
#include <cstring>

struct MtpLoginLogoutStruct {
  char user_id[20];
  char password[10];
};

///行情
struct MtpMarketDataStruct {
  char exchange_id[10];
  char instrument_id[32];
  int64_t positon;
  char date_time[24];
  int64_t volume;
  double last_price;
  double bid_price[10];
  double ask_price[10];
  int32_t bid_volume[10];
  int32_t ask_volume[10];
  bool state;
};

///行情静态信息
struct MtpInstrumentInfo {
  char exchange_id[10];
  char instrument_id[32];
  double ticksize;
  double tradeuint;
  bool is_trading;
  int32_t buy_volume_min;
  int32_t sell_volume_min;
  int32_t buy_volume_max;
  int32_t sell_volume_max;
  int32_t prid;
  bool is_last = false;
};

#endif