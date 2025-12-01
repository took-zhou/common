
#ifndef _BTP_TRADER_STRUCT_TYPE_H_
#define _BTP_TRADER_STRUCT_TYPE_H_

#include <cstdint>

struct BtpLoginLogoutStruct {
  char user_id[10];
  char password[10];
};

struct BtpOrderInfoStruct {
  enum OrderStatus { kCanceled = 1, kRejected, kNoOpened };
  char exchange_id[10];
  char instrument_id[32];
  char index[10];
  char user_id[10];
  double price;
  int volume;
  int order_ref;
  uint8_t side;       /*1 买 2 卖*/
  uint8_t comboffset; /* 1 开仓 大于1 平仓*/
  char date[12];
  char time[12];
  OrderStatus order_status;
};

struct BtpTransactionCostField {
  char exchange_id[10];
  char instrument_id[32];
  char user_id[10];
};

struct BtpCommissionInfo {
  char exchange_id[10];
  char instrument_id[32];
  char user_id[10];
  int32_t prid;
  double open_ratio_by_money;
  double open_ratio_by_volume;
  double close_ratio_by_money;
  double close_ratio_by_volume;
  double close_today_ratio_by_money;
  double close_today_ratio_by_volume;
};

struct BtpMarginInfo {
  char exchange_id[10];
  char instrument_id[32];
  char user_id[10];
  int32_t prid;
  double long_margin_ratio_by_money;
  double long_margin_ratio_by_volume;
  double short_margin_ratio_by_money;
  double short_margin_ratio_by_volume;
};

/// 行情静态信息
struct BtpAccountInfo {
  char user_id[10];
  int32_t prid;
  double balance;
  double available;
  uint64_t session_id;
};

struct BtpAccountField {
  char user_id[10];
  char password[10];
};

#endif