/////////////////////////////////////////////////////////////////////////
///@author 朝代数算量化合伙企业
///@file ftp_trader_api.h
///@brief 定义行情订阅客户端接口
/////////////////////////////////////////////////////////////////////////

#ifndef _FTP_TRADER_API_H_
#define _FTP_TRADER_API_H_

#include <memory>
#include "ftp_trader_struct.h"

/*!
 * \class FTP::API::TraderSpi
 *
 * \brief 交易回调类
 *
 * \author 朝代数算量化合伙企业
 * \date 十月 2022
 */
namespace ftp {
namespace api {
class TraderSpi {
 public:
  virtual void OnRspUserLogin(const FtpLoginLogoutStruct *login_info){};

  virtual void OnRspUserLogout(const FtpLoginLogoutStruct *logout_info){};

  virtual void OnRtnTrade(const FtpOrderInfoStruct *trade_info){};

  virtual void OnRtnOrder(const FtpOrderInfoStruct *order_info){};

  virtual void OnRtnOrderInsert(const FtpOrderInfoStruct *trade_info){};

  virtual void OnRtnOrderAction(const FtpOrderInfoStruct *order_info){};

  virtual void OnRspTradingAccount(const FtpAccountInfo *account_info){};

  virtual void OnRspMarginRate(const FtpMarginInfo *margin_info){};

  virtual void OnRspCommissionRate(const FtpCommissionInfo *commission_info){};
};
}  // namespace api
}  // namespace ftp

/*!
 * \class FTP::API::TraderApi
 *
 * \brief 交易下单接口类
 *
 * \author 朝代数算量化合伙企业
 * \date 十月 2022
 */
namespace ftp {
namespace api {
class TraderApiImpl;
class TraderApi {
 public:
  static TraderApi *CreateTraderApi(const char *path);
  void RegisterSpi(TraderSpi *spi);
  uint64_t Login(const FtpLoginLogoutStruct &login);
  uint64_t Logout(const FtpLoginLogoutStruct &logout);
  int ReqOrderInsert(const FtpOrderInfoStruct *order_info);
  int ReqOrderAction(const FtpOrderInfoStruct *order_info);
  int QryTradingAccount(FtpAccountField *field, int request_id);
  int QryTransactionCost(FtpTransactionCostField *field, int request_id);
  void Release();

 private:
  std::shared_ptr<TraderApiImpl> trader_api_impl_ = nullptr;
};
}  // namespace api
}  // namespace ftp

#endif
