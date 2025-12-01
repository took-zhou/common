#ifndef __UTILS__
#define __UTILS__

#include <stddef.h>
#include <set>
#include <string>
#include <vector>

namespace utils {
void CreatFolder(const std::string &folder_name);
void CreatFile(const std::string &file_name);
void DeleteFile(const std::string &file_name);

bool IsFileExist(const std::string &file_name);
std::vector<std::string> SplitString(const std::string &str, const std::string &pattern);
std::vector<std::string> SplitInstrument(const std::string &ins);
void CodeConvert(char *from, char *toward, char *src, char *dst, size_t dst_size);
void Gbk2Utf8(char *src, char *dst, size_t dst_size);

int GetFileSize(const std::string &filepath);
std::string GetCompileTime();
bool GetHostInfo(std::string &host_name, std::string &ip_address);
bool GetLocalMac(std::string &mac_str);
bool GetLocalIp(std::string &ip_str);

struct InstrumentID {
  std::string ins;
  std::string exch;
};

struct InstrumentIDSortCriterion {
 public:
  bool operator()(const utils::InstrumentID &ins_a, const utils::InstrumentID &ins_b) const {
    // 只以合约名作为排序依据
    return ins_a.ins < ins_b.ins;
  }
};

struct ItpMsg {
  std::string session_name = "";
  std::string msg_name = "";
  std::string pb_msg = "";
};

struct OrderContent {
  std::string group_id{"unknow"};
  std::string user_id{"unknow"};
  std::string exchange_id{"unknow"};
  std::string instrument_id{"unknow"};
  std::string order_ref{"unknow"};
  std::string index{"unknow"};
  uint64_t session_id;
  uint64_t xtp_order_id;
  uint8_t direction;  /*1 买 2 卖*/
  uint8_t comboffset; /* 1 开仓 大于1 平仓*/
  uint8_t order_type;
  uint32_t once_volume;
  uint32_t hold_volume;
  uint32_t success_volume;
  uint32_t fail_volume;
  double limit_price;
  bool active_cancle_indication;
  bool is_flow_finished;
};

struct TickContent {
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

bool ReceMsg(utils::ItpMsg &msg, const std::string &recv_string, int32_t recv_size);
int32_t GetMonday(const std::string &date);
}  // namespace utils

#endif
