#include <memory>
#include "common/self/utils.h"
#include "gtest/gtest.h"

namespace {
struct UtilsTest : public testing::Test {
 public:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(UtilsTest, split_ins) {
  std::string exch = "SHFE";
  std::string group = "TA12";
  std::string prev_date = "20251131";
  char ins[32] = {0};
  uint8_t year_number = 0;

  auto group_split = utils::SplitInstrument(group);
  if (group_split.size() != 2) {
    printf("%s is not right group name.", exch.c_str());
  }

  if (exch == "CZCE") {
    year_number = group_split[1] < prev_date.substr(4, 2) ? stoi(prev_date.substr(3, 1)) + 1 : stoi(prev_date.substr(3, 1));
    sprintf(ins, "%s%01d%s", group_split[0].c_str(), year_number % 10, group_split[1].c_str());
    printf("%s:%s->%s", group.c_str(), prev_date.c_str(), ins);
  } else {
    year_number = group_split[1] < prev_date.substr(4, 2) ? stoi(prev_date.substr(2, 2)) + 1 : stoi(prev_date.substr(2, 2));
    sprintf(ins, "%s%02d%s", group_split[0].c_str(), year_number % 100, group_split[1].c_str());
    printf("%s:%s->%s", group.c_str(), prev_date.c_str(), ins);
  }
};

TEST_F(UtilsTest, rece_msg) {
  utils::ItpMsg msg;
  const std::string recv_string = "strategy_market.TickData. lajflkjalk";
  int32_t recv_size = recv_string.size();
  utils::ReceMsg(msg, recv_string, recv_size);
  printf("%s\r\n", msg.session_name.c_str());
  printf("%s\r\n", msg.msg_name.c_str());
  printf("%s\r\n", msg.pb_msg.c_str());
};
}  // namespace