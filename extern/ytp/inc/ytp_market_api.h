/////////////////////////////////////////////////////////////////////////
///@author 朝代数算量化合伙企业
///@file ytp_market_api.h
///@brief 定义行情订阅客户端接口
/////////////////////////////////////////////////////////////////////////

#ifndef _YTP_MARKET_API_H_
#define _YTP_MARKET_API_H_

#include <memory>
#include "ytp_market_struct.h"

/*!
 * \class YTP::API::MarketSpi
 *
 * \brief 行情回调类
 *
 * \author 朝代数算量化合伙企业
 * \date 十月 2022
 */
namespace ytp {
namespace api {
class MarketSpi {
 public:
  virtual void OnDepthMarketData(const YtpMarketDataStruct *market_data){};

  virtual void OnRspUserLogin(const YtpLoginLogoutStruct *login_info){};

  virtual void OnRspUserLogout(const YtpLoginLogoutStruct *logout_info){};

  virtual void OnRspAllInstrumentInfo(YtpInstrumentInfo *ticker_info){};

  virtual void OnFrontDisconnected(int reason){};
};
}  // namespace api
}  // namespace ytp

/*!
 * \class YTP::API::MarketApi
 *
 * \brief 行情订阅接口类
 *
 * \author 朝代数算量化合伙企业
 * \date 十月 2022
 */
namespace ytp {
namespace api {
class MarketApiImpl;
class MarketApi {
 public:
  static MarketApi *CreateMarketApi(const char *path);
  void RegisterSpi(MarketSpi *spi);
  int SubscribeMarketData(char *ticker[], int count, int request_id);
  int UnSubscribeMarketData(char *ticker[], int count, int request_id);
  int QryInstrumentInfo(void);
  int Login(const YtpLoginLogoutStruct &login);
  int Logout(const YtpLoginLogoutStruct &login);
  void Release();

 private:
  std::shared_ptr<MarketApiImpl> market_api_impl_ = nullptr;
};
}  // namespace api
}  // namespace ytp

#endif
