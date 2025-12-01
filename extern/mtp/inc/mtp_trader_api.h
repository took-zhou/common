/////////////////////////////////////////////////////////////////////////
///@author 朝代数算量化合伙企业
///@file mtp_trader_api.h
///@brief 定义行情订阅客户端接口
/////////////////////////////////////////////////////////////////////////

#ifndef _mtp_TRADER_API_H_
#define _mtp_TRADER_API_H_

#include <memory>
#include "mtp_trader_struct.h"

/*!
 * \class mtp::API::TraderSpi
 *
 * \brief 交易回调类
 *
 * \author 朝代数算量化合伙企业
 * \date 十月 2024
 */
namespace mtp {
namespace api {
class TraderSpi {
 public:
  virtual void OnRspUserLogin(const MtpLoginLogoutStruct *login_info){};

  virtual void OnRspUserLogout(const MtpLoginLogoutStruct *logout_info){};

  virtual void OnRtnTrade(const MtpOrderInfoStruct *trade_info){};

  virtual void OnRtnOrder(const MtpOrderInfoStruct *order_info){};

  virtual void OnRtnOrderInsert(const MtpOrderInfoStruct *trade_info){};

  virtual void OnRtnOrderAction(const MtpOrderInfoStruct *order_info){};

  virtual void OnRspTradingAccount(const MtpAccountInfo *account_info){};

  virtual void OnRspMarginRate(const MtpMarginInfo *margin_info){};

  virtual void OnRspCommissionRate(const MtpCommissionInfo *commission_info){};

  virtual void OnFrontDisconnected(int reason){};
};
}  // namespace api
}  // namespace mtp

/*!
 * \class mtp::API::TraderApi
 *
 * \brief 交易下单接口类
 *
 * \author 朝代数算量化合伙企业
 * \date 十月 2024
 */
namespace mtp {
namespace api {
class TraderApiImpl;
class TraderApi {
 public:
  static TraderApi *CreateTraderApi(const char *path);
  void RegisterSpi(TraderSpi *spi);
  uint64_t Login(const MtpLoginLogoutStruct &login);
  uint64_t Logout(const MtpLoginLogoutStruct &logout);
  int ReqOrderInsert(const MtpOrderInfoStruct *order_info);
  int ReqOrderAction(const MtpOrderInfoStruct *order_info);
  int QryTradingAccount(MtpAccountField *field, int request_id);
  int QryTransactionCost(MtpTransactionCostField *field, int request_id);
  void Release();

 private:
  std::shared_ptr<TraderApiImpl> trader_api_impl_ = nullptr;
};
}  // namespace api
}  // namespace mtp

#endif
