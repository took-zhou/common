/*
 * fileUtil.cpp
 *
 *  Created on: 2020.11.19
 *      Author: Administrator
 */

#include <fstream>
#include <iomanip>

#include "common/extern/log/log.h"
#include "common/self/file_util.h"

namespace utils {

void JsonConfig::SetFileName(const std::string &file_name) {
  file_name_ = file_name;
  UpdateJsonContentByFileName(file_name);
}

const FifoJson &JsonConfig::GetConfig(bool re_load) {
  if (re_load) {
    UpdateJsonContentByFileName(file_name_);
  }

  return tmp_config_;
}

const FifoJson &JsonConfig::GetConfig(const std::string &module_name, const std::string &sub_name, bool re_load) {
  if (re_load) {
    UpdateJsonContentByFileName(file_name_);
  }

  return tmp_config_[module_name][sub_name];
}

const FifoJson &JsonConfig::GetDeepConfig(const std::string &module_name, const std::string &sub_name, const std::string &sub_sub_name,
                                          bool re_load) {
  if (re_load) {
    UpdateJsonContentByFileName(file_name_);
  }

  return tmp_config_[module_name][sub_name][sub_sub_name];
}

const FifoJson &JsonConfig::GetPartConfig(const std::string &module_name, bool re_load) {
  if (re_load) {
    UpdateJsonContentByFileName(file_name_);
  }

  return tmp_config_[module_name];
}

void JsonConfig::UpdateJsonContentByFileName(const std::string &file_name) {
  std::ifstream istrm(file_name, std::ios::binary);
  if (!istrm.is_open()) {
    ERROR_LOG("failed to open %s", file_name.c_str());
  } else {
    if (open_cnt_ == 0) {
      INFO_LOG("open %s ok", file_name.c_str());
      open_cnt_++;
    }
    istrm >> tmp_config_;
    istrm.close();
  }
}

bool JsonConfig::WriteConfig(const std::string &module_name, const std::string &sub_name, const std::string &value) {
  bool ret = true;
  tmp_config_[module_name][sub_name] = value;

  std::ofstream in_file(file_name_);
  if (in_file.is_open()) {
    in_file << setw(4) << tmp_config_ << endl;
  } else {
    ERROR_LOG("file:%s open error.", file_name_.c_str());
    ret = false;
  }
  in_file.close();

  return ret;
}

}  // namespace utils
