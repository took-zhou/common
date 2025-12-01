
#ifndef _GTP_MARKET_STRUCT_TYPE_H_
#define _GTP_MARKET_STRUCT_TYPE_H_

#include <cstdint>
#include <cstring>

struct GtpLoginLogoutStruct {
  char market_state[16];
  char date[16];
};

///行情
struct GtpMarketDataStruct {
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
struct GtpInstrumentInfo {
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