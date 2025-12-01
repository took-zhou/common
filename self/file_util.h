/*
 * file_util.h
 *
 *  Created on: 2020.11.19
 *      Author: Administrator
 */

#ifndef WORKSPACE_TRADE_COMMON_FILEUTIL_H_
#define WORKSPACE_TRADE_COMMON_FILEUTIL_H_
#include <string>
#include "common/extern/json/fifo_map.hpp"
#include "common/extern/json/json.h"

namespace utils {
using namespace nlohmann;
template <class K, class V, class dummy_compare, class A>
using FifoWorkaroundFifoMap = fifo_map<K, V, fifo_map_compare<K>, A>;
using FifoJson = basic_json<FifoWorkaroundFifoMap>;

struct JsonConfig {
 public:
  JsonConfig(){};
  JsonConfig(const JsonConfig &) = delete;
  JsonConfig &operator=(const JsonConfig &) = delete;
  static JsonConfig &GetInstance() {
    static JsonConfig instance;
    return instance;
  }

  const FifoJson &GetConfig(bool re_load = true);
  const FifoJson &GetConfig(const std::string &module_name, const std::string &sub_name, bool re_load = false);
  const FifoJson &GetDeepConfig(const std::string &module_name, const std::string &sub_name, const std::string &sub_sub_name,
                                bool re_load = false);
  const FifoJson &GetPartConfig(const std::string &module_name, bool re_load = false);

  void SetFileName(const std::string &file_name);
  const std::string &GetFileName(void) { return file_name_; };
  bool WriteConfig(const std::string &module_name, const std::string &sub_name, const std::string &value);

 private:
  void UpdateJsonContentByFileName(const std::string &file_name);
  FifoJson tmp_config_;
  int open_cnt_{0};
  std::string file_name_ = "";  //根据自己配置的存放目录情况修改
};

}  // namespace utils

#endif /* WORKSPACE_TRADE_COMMON_FILEUTIL_H_ */
