/////////////////////////////////////////////////////////////////////////
///@author 朝代数算量化合伙企业
///@file btp_market_api.h
///@brief 定义行情订阅客户端接口
/////////////////////////////////////////////////////////////////////////

#ifndef _BTP_MARKET_API_H_
#define _BTP_MARKET_API_H_

#include <memory>
#include "btp_market_struct.h"

/*!
 * \class BTP::API::MarketSpi
 *
 * \brief 行情回调类
 *
 * \author 朝代数算量化合伙企业
 * \date 十月 2022
 */
namespace btp {
namespace api {
class MarketSpi {
 public:
  virtual void OnDepthMarketData(const BtpMarketDataStruct *market_data){};

  virtual void OnRspUserLogin(const BtpLoginLogoutStruct *login_info){};

  virtual void OnRspUserLogout(const BtpLoginLogoutStruct *logout_info){};

  virtual void OnRspAllInstrumentInfo(BtpInstrumentInfo *ticker_info){};
};
}  // namespace api
}  // namespace btp

/*!
 * \class BTP::API::MarketApi
 *
 * \brief 行情订阅接口类
 *
 * \author 朝代数算量化合伙企业
 * \date 十月 2022
 */
namespace btp {
namespace api {
class MarketApiImpl;
class MarketApi {
 public:
  static MarketApi *CreateMarketApi(const char *path);
  void RegisterSpi(MarketSpi *spi);
  int SubscribeMarketData(char *ticker[], int count, int request_id);
  int UnSubscribeMarketData(char *ticker[], int count, int request_id);
  int QryInstrumentInfo(void);
  int Login();
  int Logout();
  void Release();

 private:
  std::shared_ptr<MarketApiImpl> market_api_impl_ = nullptr;
};
}  // namespace api
}  // namespace btp

#endif
