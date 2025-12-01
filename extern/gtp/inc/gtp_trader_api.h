/////////////////////////////////////////////////////////////////////////
///@author 朝代数算量化合伙企业
///@file gtp_trader_api.h
///@brief 定义行情订阅客户端接口
/////////////////////////////////////////////////////////////////////////

#ifndef _GTP_TRADER_API_H_
#define _GTP_TRADER_API_H_

#include <memory>
#include "gtp_trader_struct.h"

/*!
 * \class GTP::API::TraderSpi
 *
 * \brief 交易回调类
 *
 * \author 朝代数算量化合伙企业
 * \date 七月 2024
 */
namespace gtp {
namespace api {
class TraderSpi {
 public:
  virtual void OnRspUserLogin(const GtpLoginLogoutStruct *login_info){};

  virtual void OnRspUserLogout(const GtpLoginLogoutStruct *logout_info){};

  virtual void OnRtnTrade(const GtpOrderInfoStruct *trade_info){};

  virtual void OnRtnOrder(const GtpOrderInfoStruct *order_info){};

  virtual void OnRtnOrderInsert(const GtpOrderInfoStruct *trade_info){};

  virtual void OnRtnOrderAction(const GtpOrderInfoStruct *order_info){};

  virtual void OnRspTradingAccount(const GtpAccountInfo *account_info){};

  virtual void OnRspMarginRate(const GtpMarginInfo *margin_info){};

  virtual void OnRspCommissionRate(const GtpCommissionInfo *commission_info){};

  virtual void OnFrontDisconnected(int reason){};
};
}  // namespace api
}  // namespace gtp

/*!
 * \class GTP::API::TraderApi
 *
 * \brief 交易下单接口类
 *
 * \author 朝代数算量化合伙企业
 * \date 七月 2024
 */
namespace gtp {
namespace api {
class TraderApiImpl;
class TraderApi {
 public:
  static TraderApi *CreateTraderApi(const char *path);
  void RegisterSpi(TraderSpi *spi);
  uint64_t Login(const GtpLoginLogoutStruct &login);
  uint64_t Logout(const GtpLoginLogoutStruct &logout);
  int ReqOrderInsert(const GtpOrderInfoStruct *order_info);
  int ReqOrderAction(const GtpOrderInfoStruct *order_info);
  int QryTradingAccount(GtpAccountField *field, int request_id);
  int QryTransactionCost(GtpTransactionCostField *field, int request_id);
  void Release();

 private:
  std::shared_ptr<TraderApiImpl> trader_api_impl_ = nullptr;
};
}  // namespace api
}  // namespace gtp

#endif
