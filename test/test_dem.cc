#include <memory>
#include "common/self/dem.h"
#include "gtest/gtest.h"

namespace {
struct DemTest : public testing::Test {
 public:
  enum DiagnosticEventId { kApiCallFailed = 0, kLoginFailed = 1, kEventMax = 2 };

  void SetUp() override {
    dem_ = std::make_unique<Dem>();
    dem_->AddTestConfig(kApiCallFailed, 3, 3, kPass);
    dem_->AddTestConfig(kLoginFailed, 1, 1, kPass);
  }

  void TearDown() override {}
  std::unique_ptr<Dem> &GetDem() { return dem_; };

 private:
  std::unique_ptr<Dem> dem_;
};

TEST_F(DemTest, read_mask) {
  GetDem()->SetEventStatus(kApiCallFailed, kPreFail);
  GetDem()->SetEventStatus(kLoginFailed, kPreFail);
  EXPECT_EQ(GetDem()->ReadFailEventMask(), 2);
  GetDem()->SetEventStatus(kApiCallFailed, kPreFail);
  GetDem()->SetEventStatus(kLoginFailed, kPreFail);
  EXPECT_EQ(GetDem()->ReadFailEventMask(), 2);
  GetDem()->SetEventStatus(kApiCallFailed, kPreFail);
  GetDem()->SetEventStatus(kLoginFailed, kPreFail);
  EXPECT_EQ(GetDem()->ReadFailEventMask(), 3);
  GetDem()->SetEventStatus(kApiCallFailed, kPrePass);
  GetDem()->SetEventStatus(kLoginFailed, kPrePass);
  EXPECT_EQ(GetDem()->ReadFailEventMask(), 0);
  GetDem()->SetEventStatus(kApiCallFailed, kPrePass);
  GetDem()->SetEventStatus(kLoginFailed, kPrePass);
  EXPECT_EQ(GetDem()->ReadFailEventMask(), 0);
  GetDem()->SetEventStatus(kApiCallFailed, kPrePass);
  GetDem()->SetEventStatus(kLoginFailed, kPrePass);
  EXPECT_EQ(GetDem()->ReadFailEventMask(), 0);
};
}  // namespace