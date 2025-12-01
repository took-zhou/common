/////////////////////////////////////////////////////////////////////////
///@author 朝代数算量化合伙企业
///@file btp_trader_api.h
///@brief 定义行情订阅客户端接口
/////////////////////////////////////////////////////////////////////////

#ifndef _BTP_TRADER_API_H_
#define _BTP_TRADER_API_H_

#include <memory>
#include "btp_trader_struct.h"

/*!
 * \class BTP::API::TraderSpi
 *
 * \brief 交易回调类
 *
 * \author 朝代数算量化合伙企业
 * \date 十月 2022
 */
namespace btp {
namespace api {
class TraderSpi {
 public:
  virtual void OnRspUserLogin(const BtpLoginLogoutStruct *login_info){};

  virtual void OnRspUserLogout(const BtpLoginLogoutStruct *logout_info){};

  virtual void OnRtnTrade(const BtpOrderInfoStruct *trade_info){};

  virtual void OnRtnOrder(const BtpOrderInfoStruct *order_info){};

  virtual void OnRtnOrderInsert(const BtpOrderInfoStruct *trade_info){};

  virtual void OnRtnOrderAction(const BtpOrderInfoStruct *order_info){};

  virtual void OnRspTradingAccount(const BtpAccountInfo *account_info){};

  virtual void OnRspMarginRate(const BtpMarginInfo *margin_info){};

  virtual void OnRspCommissionRate(const BtpCommissionInfo *commission_info){};
};
}  // namespace api
}  // namespace btp

/*!
 * \class BTP::API::TraderApi
 *
 * \brief 交易下单接口类
 *
 * \author 朝代数算量化合伙企业
 * \date 十月 2022
 */
namespace btp {
namespace api {
class TraderApiImpl;
class TraderApi {
 public:
  static TraderApi *CreateTraderApi(const char *path);
  void RegisterSpi(TraderSpi *spi);
  uint64_t Login(const BtpLoginLogoutStruct &login);
  uint64_t Logout(const BtpLoginLogoutStruct &logout);
  int ReqOrderInsert(const BtpOrderInfoStruct *order_info);
  int ReqOrderAction(const BtpOrderInfoStruct *order_info);
  int QryTradingAccount(BtpAccountField *field, int request_id);
  int QryTransactionCost(BtpTransactionCostField *field, int request_id);
  void Release();

 private:
  std::shared_ptr<TraderApiImpl> trader_api_impl_ = nullptr;
};
}  // namespace api
}  // namespace btp

#endif
