#include <memory>
#include "common/self/file_util.h"
#include "gtest/gtest.h"

namespace {
struct FileUtilTest : public testing::Test {
 public:
  void SetUp() override {
    json_config_ = std::make_unique<utils::JsonConfig>();
    json_config_->SetFileName("/etc/marktrade/config.json");
  }

  void TearDown() override {}
  std::unique_ptr<utils::JsonConfig> &GetJsonConfig() { return json_config_; };

 private:
  std::unique_ptr<utils::JsonConfig> json_config_;
};

TEST_F(FileUtilTest, get_config) {
  EXPECT_STREQ(GetJsonConfig()->GetConfig("market", "LogPath").get<std::string>().c_str(), "/home/tsaodai/log/market/");
  EXPECT_STREQ(GetJsonConfig()->GetDeepConfig("users", "129080_simnow-allhours", "UserID").get<std::string>().c_str(), "129080");
  GetJsonConfig()->WriteConfig("market", "LogPath", "/home/tsaodai/log/market2/");
  EXPECT_STREQ(GetJsonConfig()->GetConfig("market", "LogPath").get<std::string>().c_str(), "/home/tsaodai/log/market2/");
  GetJsonConfig()->WriteConfig("market", "LogPath", "/home/tsaodai/log/market/");
  EXPECT_STREQ(GetJsonConfig()->GetConfig("market", "LogPath").get<std::string>().c_str(), "/home/tsaodai/log/market/");
};
}  // namespace