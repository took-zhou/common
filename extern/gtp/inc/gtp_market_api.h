/////////////////////////////////////////////////////////////////////////
///@author 朝代数算量化合伙企业
///@file gtp_market_api.h
///@brief 定义行情订阅客户端接口
/////////////////////////////////////////////////////////////////////////

#ifndef _GTP_MARKET_API_H_
#define _GTP_MARKET_API_H_

#include <memory>
#include "gtp_market_struct.h"

/*!
 * \class GTP::API::MarketSpi
 *
 * \brief 行情回调类
 *
 * \author 朝代数算量化合伙企业
 * \date 七月 2024
 */
namespace gtp {
namespace api {
class MarketSpi {
 public:
  virtual void OnDepthMarketData(const GtpMarketDataStruct *market_data){};

  virtual void OnRspUserLogin(const GtpLoginLogoutStruct *login_info){};

  virtual void OnRspUserLogout(const GtpLoginLogoutStruct *logout_info){};

  virtual void OnRspAllInstrumentInfo(GtpInstrumentInfo *ticker_info){};

  virtual void OnFrontDisconnected(int reason){};
};
}  // namespace api
}  // namespace gtp

/*!
 * \class GTP::API::MarketApi
 *
 * \brief 行情订阅接口类
 *
 * \author 朝代数算量化合伙企业
 * \date 七月 2024
 */
namespace gtp {
namespace api {
class MarketApiImpl;
class MarketApi {
 public:
  static MarketApi *CreateMarketApi(const char *path = nullptr);
  void RegisterSpi(MarketSpi *spi);
  int SubscribeMarketData(char *ticker[], int count, int request_id);
  int UnSubscribeMarketData(char *ticker[], int count, int request_id);
  int QryInstrumentInfo(void);
  int Login(const GtpLoginLogoutStruct &login);
  int Logout(const GtpLoginLogoutStruct &logout);
  void Release();

 private:
  std::shared_ptr<MarketApiImpl> market_api_impl_ = nullptr;
};
}  // namespace api
}  // namespace gtp

#endif
