/////////////////////////////////////////////////////////////////////////
///@author 朝代数算量化合伙企业
///@file ytp_trader_api.h
///@brief 定义行情订阅客户端接口
/////////////////////////////////////////////////////////////////////////

#ifndef _YTP_TRADER_API_H_
#define _YTP_TRADER_API_H_

#include <memory>
#include "ytp_trader_struct.h"

/*!
 * \class YTP::API::TraderSpi
 *
 * \brief 交易回调类
 *
 * \author 朝代数算量化合伙企业
 * \date 十月 2022
 */
namespace ytp {
namespace api {
class TraderSpi {
 public:
  virtual void OnRspUserLogin(const YtpLoginLogoutStruct *login_info){};

  virtual void OnRspUserLogout(const YtpLoginLogoutStruct *logout_info){};

  virtual void OnRtnTrade(const YtpOrderInfoStruct *trade_info){};

  virtual void OnRtnOrder(const YtpOrderInfoStruct *order_info){};

  virtual void OnRtnOrderInsert(const YtpOrderInfoStruct *trade_info){};

  virtual void OnRtnOrderAction(const YtpOrderInfoStruct *order_info){};

  virtual void OnRspTradingAccount(const YtpAccountInfo *account_info){};

  virtual void OnRspMarginRate(const YtpMarginInfo *margin_info){};

  virtual void OnRspCommissionRate(const YtpCommissionInfo *commission_info){};

  virtual void OnFrontDisconnected(int reason){};
};
}  // namespace api
}  // namespace ytp

/*!
 * \class YTP::API::TraderApi
 *
 * \brief 交易下单接口类
 *
 * \author 朝代数算量化合伙企业
 * \date 十月 2022
 */
namespace ytp {
namespace api {
class TraderApiImpl;
class TraderApi {
 public:
  static TraderApi *CreateTraderApi(const char *path);
  void RegisterSpi(TraderSpi *spi);
  uint64_t Login(const YtpLoginLogoutStruct &login);
  uint64_t Logout(const YtpLoginLogoutStruct &logout);
  int ReqOrderInsert(const YtpOrderInfoStruct *order_info);
  int ReqOrderAction(const YtpOrderInfoStruct *order_info);
  int QryTradingAccount(YtpAccountField *field, int request_id);
  int QryTransactionCost(YtpTransactionCostField *field, int request_id);
  void Release();

 private:
  std::shared_ptr<TraderApiImpl> trader_api_impl_ = nullptr;
};
}  // namespace api
}  // namespace ytp

#endif
