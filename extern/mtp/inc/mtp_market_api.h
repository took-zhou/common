/////////////////////////////////////////////////////////////////////////
///@author 朝代数算量化合伙企业
///@file mtp_market_api.h
///@brief 定义行情订阅客户端接口
/////////////////////////////////////////////////////////////////////////

#ifndef _MTP_MARKET_API_H_
#define _MTP_MARKET_API_H_

#include <memory>
#include "mtp_market_struct.h"

/*!
 * \class MTP::API::MarketSpi
 *
 * \brief 行情回调类
 *
 * \author 朝代数算量化合伙企业
 * \date 十月 2024
 */
namespace mtp {
namespace api {
class MarketSpi {
 public:
  virtual void OnDepthMarketData(const MtpMarketDataStruct *market_data){};

  virtual void OnRspUserLogin(const MtpLoginLogoutStruct *login_info){};

  virtual void OnRspUserLogout(const MtpLoginLogoutStruct *logout_info){};

  virtual void OnRspAllInstrumentInfo(MtpInstrumentInfo *ticker_info){};

  virtual void OnFrontDisconnected(int reason){};
};
}  // namespace api
}  // namespace mtp

/*!
 * \class MTP::API::MarketApi
 *
 * \brief 行情订阅接口类
 *
 * \author 朝代数算量化合伙企业
 * \date 十月 2024
 */
namespace mtp {
namespace api {
class MarketApiImpl;
class MarketApi {
 public:
  static MarketApi *CreateMarketApi(const char *path = nullptr);
  void RegisterSpi(MarketSpi *spi);
  int SubscribeMarketData(char *ticker[], int count, int request_id);
  int UnSubscribeMarketData(char *ticker[], int count, int request_id);
  int QryInstrumentInfo(void);
  int Login(const MtpLoginLogoutStruct &login);
  int Logout(const MtpLoginLogoutStruct &logout);
  void Release();

 private:
  std::shared_ptr<MarketApiImpl> market_api_impl_ = nullptr;
};
}  // namespace api
}  // namespace mtp

#endif
