/////////////////////////////////////////////////////////////////////////
///@author 朝代数算量化合伙企业
///@file ftp_market_api.h
///@brief 定义行情订阅客户端接口
/////////////////////////////////////////////////////////////////////////

#ifndef _FTP_MARKET_API_H_
#define _FTP_MARKET_API_H_

#include <memory>
#include "ftp_market_struct.h"

/*!
 * \class FTP::API::MarketSpi
 *
 * \brief 行情回调类
 *
 * \author 朝代数算量化合伙企业
 * \date 十月 2022
 */
namespace ftp {
namespace api {
class MarketSpi {
 public:
  virtual void OnDepthMarketData(const FtpMarketDataStruct *market_data){};

  virtual void OnRspUserLogin(const FtpLoginLogoutStruct *login_info){};

  virtual void OnRspUserLogout(const FtpLoginLogoutStruct *logout_info){};

  virtual void OnRspAllInstrumentInfo(FtpInstrumentInfo *ticker_info){};
};
}  // namespace api
}  // namespace ftp

/*!
 * \class FTP::API::MarketApi
 *
 * \brief 行情订阅接口类
 *
 * \author 朝代数算量化合伙企业
 * \date 十月 2022
 */
namespace ftp {
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
}  // namespace ftp

#endif
