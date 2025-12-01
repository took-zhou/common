#include <memory>
#include "common/self/global_sem.h"
#include "gtest/gtest.h"

namespace {
struct GlobalSemTest : public testing::Test {
 public:
  void SetUp() override { global_sem_ = std::make_unique<GlobalSem>(); }

  void TearDown() override {}
  std::unique_ptr<GlobalSem> &GetGlobalSem() { return global_sem_; };

 private:
  std::unique_ptr<GlobalSem> global_sem_;
};

TEST_F(GlobalSemTest, post_wait) {
  EXPECT_TRUE(GetGlobalSem()->ExistSem(kMarketRsp));
  EXPECT_TRUE(GetGlobalSem()->ExistSem(kLoginLogout));
  GetGlobalSem()->PostSemBySemName(kMarketRsp);
  EXPECT_EQ(GetGlobalSem()->WaitSemBySemName(kMarketRsp), 0);
  EXPECT_NE(GetGlobalSem()->WaitSemBySemName(kLoginLogout, 3), 0);
};
}  // namespace