
#include "common/self/utils.h"
#include <iconv.h>
#include <stdio.h>
#include <string.h>
#include "common/extern/log/log.h"
#include "common/self/file_util.h"

#include <arpa/inet.h> /* inet_ntop */
#include <net/if.h>
#include <net/if_arp.h>
#include <netdb.h> /* struct hostent */
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>/* gethostname */
#include <sstream>

#include <sys/stat.h>

namespace utils {
vector<string> Split(const string &str, const string &pattern);
bool RemoveSpecialStrInTheEnd(string &str, const char &pattern);
bool AddCntToOrderRef(string &order_ref, const unsigned int &cnt);

void CodeConvert(const char *from, const char *toward, char *src, char *dst, size_t dst_size) {
  size_t src_length, dst_length;
  src_length = strlen(src);
  dst_length = dst_size;
  char *in_ptr = src;
  char *out_ptr = dst;
  memset(dst, '\0', dst_size);
  iconv_t conv = iconv_open(toward, from);
  iconv(conv, &in_ptr, &src_length, &out_ptr, &dst_length);
  iconv_close(conv);
}

void Gbk2Utf8(char *src, char *dst, size_t dst_size) { CodeConvert("GBK", "UTF-8", src, dst, dst_size); }

std::vector<std::string> SplitString(const std::string &str, const std::string &pattern) {
  std::vector<std::string> res;
  if (str == "") return res;
  std::string strs = str + pattern;
  size_t pos = strs.find(pattern);
  while (pos != strs.npos) {
    std::string temp = strs.substr(0, pos);
    res.push_back(temp);
    strs = strs.substr(pos + pattern.size(), strs.size());
    pos = strs.find(pattern);
  }
  return res;
}

std::vector<std::string> SplitInstrument(const std::string &ins) {
  struct Index {
    int start = 0;
    int end = 0;
  };
  std::vector<std::string> temp_split_list;
  enum ValueType { kReserve = 0, kDigital = 1, kLow = 2, kHigh = 3 };
  ValueType type = kReserve;
  Index index;
  for (int count = 0; count < ins.size(); count++) {
    if ('0' <= ins[count] && ins[count] <= '9') {
      if (type == kReserve) {
        index.start = count;
        index.end = count;
      } else if (type != kDigital) {
        temp_split_list.push_back(ins.substr(index.start, index.end - index.start + 1));
        index.start = count;
        index.end = count;
      } else {
        index.end = count;
      }
      type = kDigital;
    } else if ('a' <= ins[count] && ins[count] <= 'z') {
      if (type == kReserve) {
        index.start = count;
        index.end = count;
      } else if (type != kLow) {
        temp_split_list.push_back(ins.substr(index.start, index.end - index.start + 1));
        index.start = count;
        index.end = count;
      } else {
        index.end = count;
      }
      type = kLow;
    } else if ('A' <= ins[count] && ins[count] <= 'Z') {
      if (type == kReserve) {
        index.start = count;
        index.end = count;
      } else if (type != kHigh) {
        temp_split_list.push_back(ins.substr(index.start, index.end - index.start + 1));
        index.start = count;
        index.end = count;
      } else {
        index.end = count;
      }
      type = kHigh;
    }
  }
  temp_split_list.push_back(ins.substr(index.start, index.end - index.start + 1));

  return temp_split_list;
}

std::vector<std::string> SplitDirAndFileName(const std::string &str) {
  std::vector<std::string> dir_and_file;
  if (str == "") {
    return dir_and_file;
  }
  bool is_absolute_path{false};
  if (str[0] == '/') {
    is_absolute_path = true;
  }
  auto splited_str = SplitString(str, "/");
  if (splited_str.size() < 1) {
    ERROR_LOG("%s", "input dir and file name str is error");
    return dir_and_file;
  }
  if (splited_str.size() == 1) {
    dir_and_file.emplace_back(std::string(""));
    dir_and_file.emplace_back(splited_str[0]);
    return dir_and_file;
  }
  auto vec_size = splited_str.size();
  std::string tmp_dir = "";
  if (is_absolute_path) {
    tmp_dir = "/";
  }
  for (decltype(vec_size) i = 0; i < vec_size - 1; ++i) {
    if (splited_str[i] == "" || splited_str[i] == " ") {
      continue;
    }
    tmp_dir = tmp_dir + (splited_str[i] + "/");
  }
  dir_and_file.emplace_back(tmp_dir);
  dir_and_file.emplace_back(splited_str[vec_size - 1]);
  return dir_and_file;
}

void CreatFolder(const std::string &folder_name) {
  if (access(folder_name.c_str(), 0) == -1) {
    std::string command = "mkdir -p " + folder_name;
    if (system(command.c_str()) != 0) {
      ERROR_LOG("create dir[%s] fail", folder_name.c_str());
    }
  }
}

void CreatFile(const std::string &file_name) {
  if (access(file_name.c_str(), 0) == -1) {
    std::vector<std::string> dir_and_file = SplitDirAndFileName(file_name);
    if (dir_and_file[0] != "") {
      CreatFolder(dir_and_file[0]);
    }
    std::string command = "touch " + file_name;
    if (system(command.c_str()) != 0) {
      ERROR_LOG("create file[%s] fail", file_name.c_str());
    }
  }
}

void DeleteFile(const std::string &file_name) {
  if (access(file_name.c_str(), 0) == 0) {
    std::string command = "rm " + file_name;
    if (system(command.c_str()) != 0) {
      ERROR_LOG("delete file[%s] fail", file_name.c_str());
    }
  }
}

bool IsFileExist(const std::string &file_name) { return access(file_name.c_str(), 0) != -1; }

int GetFileSize(const std::string &filepath) {
  struct stat statbuf;
  if (stat(filepath.c_str(), &statbuf) == 0) return statbuf.st_size;
  return -1;
}

bool RemoveSpecialStrInTheEnd(string &str, const char &pattern) {
  int string_len = str.length();
  if (str.at(string_len - 1) == pattern) {
    str = str.substr(0, string_len - 1);
  }
  return true;
}

vector<string> Split(const string &str, const string &pattern) {
  vector<string> res;
  if (str == "") return res;
  string strs = str + pattern;
  size_t pos = strs.find(pattern);
  while (pos != strs.npos) {
    string temp = strs.substr(0, pos);
    res.push_back(temp);
    strs = strs.substr(pos + 1, strs.size());
    pos = strs.find(pattern);
  }
  return res;
}

/***********************************************/
std::string GetCompileTime() {
  std::string ach_monthes[] = {
      "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
  };

  std::string str_date = __DATE__;
  int year = atoi(str_date.substr(str_date.length() - 4).c_str());
  int month = 0;
  for (int i = 0; i < 12; i++) {
    if (str_date.find(ach_monthes[i]) != string::npos) {
      month = i + 1;
      break;
    }
  }

  std::string day_str = str_date.substr(4, 2);
  int day = atoi(day_str.c_str());
  std::string str_time = __TIME__;
  char ach_time_desc[20];
  sprintf(ach_time_desc, "%04d-%02d-%02d %s", year, month, day, str_time.c_str());
  return std::string(ach_time_desc);
}

bool GetHostInfo(std::string &host_name, std::string &ip_address) {
  char name[256];
  gethostname(name, sizeof(name));  // @suppress("Invalid arguments")
  host_name = name;

  struct hostent *host = gethostbyname(name);  // @suppress("Invalid arguments")
  if (host == nullptr) {
    std::cout << "get hostname failed" << std::endl;
    return false;
  }
  char ip_str[32];
  const char *ret = inet_ntop(host->h_addrtype, host->h_addr_list[0], ip_str, sizeof(ip_str));
  if (ret == nullptr) {
    std::cout << "hostname transform to ip failed" << std::endl;
    return false;
  }
  ip_address = ip_str;
  return true;
}

bool GetLocalMac(std::string &mac_str) {
  //    std::string macStr;
  int sockfd;
  struct ifreq tmp;
  char mac_addr[30];
  char mac[30];
  sockfd = socket(AF_INET, SOCK_STREAM, 0);  // @suppress("Symbol is not resolved") //
                                             // @suppress("Invalid arguments")
  if (sockfd < 0) {
    ERROR_LOG("create socket fail\n");
    return false;
  }

  memset(&tmp, 0, sizeof(struct ifreq));
  auto &json_cfg = utils::JsonConfig::GetInstance();
  auto net_board = json_cfg.GetConfig("trader", "NetBoard").get<std::string>();
  strncpy(tmp.ifr_name, net_board.c_str(),
          sizeof(tmp.ifr_name) - 1);             // @suppress("Field cannot be resolved") //
                                                 // @suppress("Invalid arguments")
  if ((ioctl(sockfd, SIOCGIFHWADDR, &tmp)) < 0)  // @suppress("Symbol is not resolved") // @suppress("Function cannot be
                                                 // resolved")
  {
    ERROR_LOG("mac ioctl error\n");
    return false;
  }

  sprintf(mac_addr, "%02x:%02x:%02x:%02x:%02x:%02x", static_cast<unsigned char>(tmp.ifr_hwaddr.sa_data[0]),
          static_cast<unsigned char>(tmp.ifr_hwaddr.sa_data[1]), static_cast<unsigned char>(tmp.ifr_hwaddr.sa_data[2]),
          static_cast<unsigned char>(tmp.ifr_hwaddr.sa_data[3]), static_cast<unsigned char>(tmp.ifr_hwaddr.sa_data[4]),
          static_cast<unsigned char>(tmp.ifr_hwaddr.sa_data[5]));
  close(sockfd);
  memcpy(mac, mac_addr, strlen(mac_addr));
  mac[17] = '\0';
  mac_str = std::string(mac);
  return true;
}

bool GetLocalIp(std::string &ip_str) {
  bool ret = true;
  int inet_sock;
  struct ifreq ifr;
  inet_sock = socket(AF_INET, SOCK_DGRAM, 0);

  strcpy(ifr.ifr_name, "eth0");
  // SIOCGIFADDR标志代表获取接口地址
  if (ioctl(inet_sock, SIOCGIFADDR, &ifr) < 0) {
    ERROR_LOG("ioctl error!");
    ret = false;
  }

  ip_str = inet_ntoa((reinterpret_cast<struct sockaddr_in *>(&(ifr.ifr_addr)))->sin_addr);
  return ret;
}

bool ReceMsg(utils::ItpMsg &msg, const std::string &recv_string, int32_t recv_size) {
  if (recv_size == -1) {
    return false;
  }

  int start_index = 0;
  int seg_index = 0;

  for (int i = 0; i < recv_size; i++) {
    if (recv_string[i] == '.') {
      if (seg_index == 0) {
        msg.session_name.resize(i - start_index);
        memcpy(&msg.session_name[0], &recv_string[start_index], (i - start_index));
      } else if (seg_index == 1) {
        msg.msg_name.resize(i - start_index);
        memcpy(&msg.msg_name[0], &recv_string[start_index], (i - start_index));
      }
      start_index = i + 1;
      seg_index++;
    } else if (recv_string[i] == ' ') {
      if (seg_index == 1) {
        msg.msg_name.resize(i - start_index);
        memcpy(&msg.msg_name[0], &recv_string[start_index], (i - start_index));
      }
      start_index = i + 1;
      break;
    }
  }

  msg.pb_msg.resize(recv_size - start_index);
  memcpy(&msg.pb_msg[0], &recv_string[start_index], (recv_size - start_index));

  return true;
}

int32_t GetMonday(const std::string &date) {
  struct tm t_now1;
  time_t t_of_day1;
  t_now1.tm_year = stoi(date) / 10000 - 1900;
  t_now1.tm_mon = stoi(date) / 100 % 100 - 1;
  t_now1.tm_mday = stoi(date) % 100;
  t_now1.tm_hour = 0;
  t_now1.tm_min = 0;
  t_now1.tm_sec = 0;
  t_now1.tm_isdst = 0;
  t_of_day1 = mktime(&t_now1);
  struct tm *tmp1 = localtime(&t_of_day1);
  int week1 = tmp1->tm_wday;
  if (week1 == 0) week1 = 7;
  int day1 = week1 - 1;
  t_of_day1 -= day1 * 24 * 3600;

  return t_of_day1;
}
}  // namespace utils
