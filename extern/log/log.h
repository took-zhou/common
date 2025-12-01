#ifndef __RING_LOG_H__
#define __RING_LOG_H__

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>  //getpid, gettid
#include <time.h>
#include <cstring>
#include <string>
#include <vector>

enum LOG_LEVEL {
  FATAL = 1,
  ERROR,
  WARN,
  INFO,
  DEBUG,
  TRACE,
};

extern pid_t gettid();

struct utc_timer {
  utc_timer() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    // set _sys_acc_sec, _sys_acc_min
    _sys_acc_sec = tv.tv_sec;
    _sys_acc_min = _sys_acc_sec / 60;
    // use _sys_acc_sec calc year, mon, day, hour, min, sec
    struct tm cur_tm;
    localtime_r((time_t*)&_sys_acc_sec, &cur_tm);
    year = cur_tm.tm_year + 1900;
    mon = cur_tm.tm_mon + 1;
    day = cur_tm.tm_mday;
    hour = cur_tm.tm_hour;
    min = cur_tm.tm_min;
    sec = cur_tm.tm_sec;
    reset_utc_fmt();
  }

  uint64_t get_curr_time(int* p_msec = NULL) {
    struct timeval tv;
    // get current ts
    gettimeofday(&tv, NULL);
    if (p_msec) *p_msec = tv.tv_usec / 1000;
    // if not in same seconds
    if ((uint32_t)tv.tv_sec != _sys_acc_sec) {
      sec = tv.tv_sec % 60;
      _sys_acc_sec = tv.tv_sec;
      // or if not in same minutes
      if (_sys_acc_sec / 60 != _sys_acc_min) {
        // use _sys_acc_sec update year, mon, day, hour, min, sec
        _sys_acc_min = _sys_acc_sec / 60;
        struct tm cur_tm;
        localtime_r((time_t*)&_sys_acc_sec, &cur_tm);
        year = cur_tm.tm_year + 1900;
        mon = cur_tm.tm_mon + 1;
        day = cur_tm.tm_mday;
        hour = cur_tm.tm_hour;
        min = cur_tm.tm_min;
        // reformat utc format
        reset_utc_fmt();
      } else {
        // reformat utc format only sec
        reset_utc_fmt_sec();
      }
    }
    return tv.tv_sec;
  }

  int year, mon, day, hour, min, sec;
  char utc_fmt[20];

 private:
  void reset_utc_fmt() { snprintf(utc_fmt, 20, "%d-%02d-%02d %02d:%02d:%02d", year, mon, day, hour, min, sec); }

  void reset_utc_fmt_sec() { snprintf(utc_fmt + 17, 3, "%02d", sec); }

  uint64_t _sys_acc_min;
  uint64_t _sys_acc_sec;
};

class cell_buffer {
 public:
  enum buffer_status { FREE, FULL };

  cell_buffer(uint32_t len) : status(FREE), prev(NULL), next(NULL), _total_len(len), _used_len(0) {
    _data = new char[len];
    if (!_data) {
      fprintf(stderr, "no space to allocate _data\n");
      exit(1);
    }
  }

  uint32_t avail_len() const { return _total_len - _used_len; }

  bool empty() const { return _used_len == 0; }

  void append(const char* log_line, uint32_t len) {
    if (avail_len() < len) return;
    memcpy(_data + _used_len, log_line, len);
    _used_len += len;
  }

  void clear() {
    _used_len = 0;
    status = FREE;
  }

  void persist(FILE* fp) {
    uint32_t wt_len = fwrite(_data, 1, _used_len, fp);
    if (wt_len != _used_len) {
      fprintf(stderr, "write log to disk error, wt_len %u\n", wt_len);
    }
  }

  buffer_status status;

  cell_buffer* prev;
  cell_buffer* next;

 private:
  cell_buffer(const cell_buffer&);
  cell_buffer& operator=(const cell_buffer&);

  uint32_t _total_len;
  uint32_t _used_len;
  char* _data;
};

class ring_log {
 public:
  // for thread-safe singleton
  static ring_log* ins() {
    pthread_once(&_once, ring_log::init);
    return _ins;
  }

  static void init() {
    while (!_ins) _ins = new ring_log();
  }

  void init_path(const char* log_dir, const char* prog_name, int level);

  int get_level() const { return _level; }

  void persist();

  void try_append(const char* lvl, const char* format, ...);

 private:
  ring_log();

  bool decis_file(int year, int mon, int day);

  ring_log(const ring_log&);
  const ring_log& operator=(const ring_log&);

  int _buff_cnt;

  cell_buffer* _curr_buf;
  cell_buffer* _prst_buf;

  cell_buffer* last_buf;

  FILE* _fp;
  pid_t _pid;
  int _year, _mon, _day, _log_cnt;
  char _prog_name[128];
  char _log_dir[512];

  bool _env_ok;  // if log dir ok
  int _level;
  uint64_t _lst_lts;  // last can't log error time(s) if value != 0, log error happened last time

  utc_timer _tm;

  static pthread_mutex_t _mutex;
  static pthread_cond_t _cond;

  static uint32_t _one_buff_len;

  // singleton
  static ring_log* _ins;
  static pthread_once_t _once;
};

void* be_thdo(void* args);

inline std::string FILE_NAME_LOG_SPLIT(const char* filename) {
  std::string str(filename);
  const std::string pattern = "/";
  std::vector<std::string> res;
  if (str == "") return res[res.size() - 1].c_str();
  std::string strs = str + pattern;
  size_t pos = strs.find(pattern);
  while (pos != strs.npos) {
    std::string temp = strs.substr(0, pos);
    res.push_back(temp);
    strs = strs.substr(pos + 1, strs.size());
    pos = strs.find(pattern);
  }
  return res[res.size() - 1];
}
inline std::string getSystemLocalTime() {
  // system time
  time_t now_time = time(NULL);
  // local time
  tm* local_time = localtime(&now_time);
  std::string sstr_localtime = asctime(local_time);
  sstr_localtime.pop_back();
  return sstr_localtime;
}

//**************************display**************************************//
/*
前景色(字体)
30      Black
31      Red
32      Green
33      Yellow
34      Blue
35      Magenta
36      Cyan
37      White
背景色
bg:
40      Black
41      Red
42      Green
43      Yellow
44      Blue
45      Magenta
46      Cyan
47      White
注：两种颜色索引值一致，一个是30开始，一个是40开始
*/

using namespace std;
#define __LOG(...)       \
  do {                   \
    printf(__VA_ARGS__); \
  } while (0)

#define __format(__fmt__) "%d " __fmt__ "\n"

#define LOG(__fmt__, ...)                              \
  do {                                                 \
    __LOG(__format(__fmt__), __LINE__, ##__VA_ARGS__); \
  } while (0)
#define GETTIME getSystemLocalTime()
//#define FILE_NAME_LOG(file)  logsplit(file)
//#define FILE_NAME_LOG(file)  file

#define RESET_C0LOR printf("\033[0m");
#define ERROR_LOG_DISPLAY(format, ...)                                                                                                  \
  do {                                                                                                                                  \
    fprintf(stdout, "\033[31m[%s|%s|%s@%s,%d] " format "\n", "error", GETTIME.c_str(), __func__, FILE_NAME_LOG_SPLIT(__FILE__).c_str(), \
            __LINE__, ##__VA_ARGS__);                                                                                                   \
    RESET_C0LOR                                                                                                                         \
  } while (0)

#define INFO_LOG_DISPLAY(format, ...)                                                                                                   \
  do {                                                                                                                                  \
    fprintf(stdout, "\033[32m[%s|%s|%s@%s,%d] " format "\n", "infor", GETTIME.c_str(), __func__, FILE_NAME_LOG_SPLIT(__FILE__).c_str(), \
            __LINE__, ##__VA_ARGS__);                                                                                                   \
    RESET_C0LOR                                                                                                                         \
  } while (0)

#define DEBUG_LOG_DISPLAY(format, ...)                                                                                                  \
  do {                                                                                                                                  \
    fprintf(stdout, "\033[35m[%s|%s|%s@%s,%d] " format "\n", "debug", GETTIME.c_str(), __func__, FILE_NAME_LOG_SPLIT(__FILE__).c_str(), \
            __LINE__, ##__VA_ARGS__);                                                                                                   \
    RESET_C0LOR                                                                                                                         \
  } while (0)

#define WARNING_LOG_DISPLAY(format, ...)                                                                                               \
  do {                                                                                                                                 \
    fprintf(stdout, "\033[33m[%s|%s|%s@%s,%d] " format "\n", "warn", GETTIME.c_str(), __func__, FILE_NAME_LOG_SPLIT(__FILE__).c_str(), \
            __LINE__, ##__VA_ARGS__);                                                                                                  \
    RESET_C0LOR                                                                                                                        \
  } while (0)

#define MSG_LOG_DISPLAY(format, ...)                                                                                                   \
  do {                                                                                                                                 \
    fprintf(stdout, "\033[37m[%s|%s|%s@%s,%d] " format "\n", "warn", GETTIME.c_str(), __func__, FILE_NAME_LOG_SPLIT(__FILE__).c_str(), \
            __LINE__, ##__VA_ARGS__);                                                                                                  \
    RESET_C0LOR                                                                                                                        \
  } while (0)
//*****************************************************************//

//#define JSON_DUMP(jsonObject) jsonObject.dump(4).c_str()
#define LINE_FEEDS fprintf(stdout, "\n");
//#define JsonPrintDisplay(jsonObject) fprintf(stdout, "\033[37m%s",jsonObject.dump(4).c_str()); LINE_FEEDS

#define LOG_MEM_SET(mem_lmt)                   \
  do {                                         \
    if (mem_lmt < 90 * 1024 * 1024) {          \
      mem_lmt = 90 * 1024 * 1024;              \
    } else if (mem_lmt > 1024 * 1024 * 1024) { \
      mem_lmt = 1024 * 1024 * 1024;            \
    }                                          \
    ring_log::_one_buff_len = mem_lmt;         \
  } while (0)

#define LOG_INIT(log_dir, prog_name, level)                \
  do {                                                     \
    ring_log::ins()->init_path(log_dir, prog_name, level); \
    pthread_t tid;                                         \
    pthread_create(&tid, NULL, be_thdo, NULL);             \
    pthread_detach(tid);                                   \
  } while (0)

// format: [LEVEL][yy-mm-dd h:m:s.ms][tid]file_name:line_no(func_name):content
#define LOG_TRACE(fmt, args...)                                                                                                     \
  do {                                                                                                                              \
    if (ring_log::ins()->get_level() >= TRACE) {                                                                                    \
      ring_log::ins()->try_append("[TRACE]", "[%u]%s:%d(%s): " fmt "\n", gettid(), FILE_NAME_LOG_SPLIT(__FILE__).c_str(), __LINE__, \
                                  __FUNCTION__, ##args);                                                                            \
    }                                                                                                                               \
  } while (0)

#define LOG_DEBUG(fmt, args...)                                                                                                     \
  do {                                                                                                                              \
    if (ring_log::ins()->get_level() >= DEBUG) {                                                                                    \
      ring_log::ins()->try_append("[DEBUG]", "[%u]%s:%d(%s): " fmt "\n", gettid(), FILE_NAME_LOG_SPLIT(__FILE__).c_str(), __LINE__, \
                                  __FUNCTION__, ##args);                                                                            \
    }                                                                                                                               \
  } while (0)

#define DEBUG_LOG(fmt, args...)                                                                                                     \
  do {                                                                                                                              \
    if (ring_log::ins()->get_level() >= DEBUG) {                                                                                    \
      ring_log::ins()->try_append("[DEBUG]", "[%u]%s:%d(%s): " fmt "\n", gettid(), FILE_NAME_LOG_SPLIT(__FILE__).c_str(), __LINE__, \
                                  __FUNCTION__, ##args);                                                                            \
    }                                                                                                                               \
    DEBUG_LOG_DISPLAY(fmt, ##args);                                                                                                 \
  } while (0)

#define LOG_INFO(fmt, args...)                                                                                                     \
  do {                                                                                                                             \
    if (ring_log::ins()->get_level() >= INFO) {                                                                                    \
      ring_log::ins()->try_append("[INFO]", "[%u]%s:%d(%s): " fmt "\n", gettid(), FILE_NAME_LOG_SPLIT(__FILE__).c_str(), __LINE__, \
                                  __FUNCTION__, ##args);                                                                           \
    }                                                                                                                              \
  } while (0)

#define INFO_LOG(fmt, args...)                                                                                                     \
  do {                                                                                                                             \
    if (ring_log::ins()->get_level() >= INFO) {                                                                                    \
      ring_log::ins()->try_append("[INFO]", "[%u]%s:%d(%s): " fmt "\n", gettid(), FILE_NAME_LOG_SPLIT(__FILE__).c_str(), __LINE__, \
                                  __FUNCTION__, ##args);                                                                           \
    }                                                                                                                              \
    INFO_LOG_DISPLAY(fmt, ##args);                                                                                                 \
  } while (0)

#define LOG_NORMAL(fmt, args...)                                                                                                   \
  do {                                                                                                                             \
    if (ring_log::ins()->get_level() >= INFO) {                                                                                    \
      ring_log::ins()->try_append("[INFO]", "[%u]%s:%d(%s): " fmt "\n", gettid(), FILE_NAME_LOG_SPLIT(__FILE__).c_str(), __LINE__, \
                                  __FUNCTION__, ##args);                                                                           \
    }                                                                                                                              \
  } while (0)

#define LOG_WARN(fmt, args...)                                                                                                     \
  do {                                                                                                                             \
    if (ring_log::ins()->get_level() >= WARN) {                                                                                    \
      ring_log::ins()->try_append("[WARN]", "[%u]%s:%d(%s): " fmt "\n", gettid(), FILE_NAME_LOG_SPLIT(__FILE__).c_str(), __LINE__, \
                                  __FUNCTION__, ##args);                                                                           \
    }                                                                                                                              \
  } while (0)

#define WARNING_LOG(fmt, args...)                                                                                                  \
  do {                                                                                                                             \
    if (ring_log::ins()->get_level() >= WARN) {                                                                                    \
      ring_log::ins()->try_append("[WARN]", "[%u]%s:%d(%s): " fmt "\n", gettid(), FILE_NAME_LOG_SPLIT(__FILE__).c_str(), __LINE__, \
                                  __FUNCTION__, ##args);                                                                           \
    }                                                                                                                              \
    WARNING_LOG_DISPLAY(fmt, ##args);                                                                                              \
  } while (0)

#define LOG_ERROR(fmt, args...)                                                                                                     \
  do {                                                                                                                              \
    if (ring_log::ins()->get_level() >= ERROR) {                                                                                    \
      ring_log::ins()->try_append("[ERROR]", "[%u]%s:%d(%s): " fmt "\n", gettid(), FILE_NAME_LOG_SPLIT(__FILE__).c_str(), __LINE__, \
                                  __FUNCTION__, ##args);                                                                            \
    }                                                                                                                               \
  } while (0)

#define ERROR_LOG(fmt, args...)                                                                                                     \
  do {                                                                                                                              \
    if (ring_log::ins()->get_level() >= ERROR) {                                                                                    \
      ring_log::ins()->try_append("[ERROR]", "[%u]%s:%d(%s): " fmt "\n", gettid(), FILE_NAME_LOG_SPLIT(__FILE__).c_str(), __LINE__, \
                                  __FUNCTION__, ##args);                                                                            \
    }                                                                                                                               \
    ERROR_LOG_DISPLAY(fmt, ##args);                                                                                                 \
  } while (0)

#define LOG_FATAL(fmt, args...)                                                                                                   \
  do {                                                                                                                            \
    ring_log::ins()->try_append("[FATAL]", "[%u]%s:%d(%s): " fmt "\n", gettid(), FILE_NAME_LOG_SPLIT(__FILE__).c_str(), __LINE__, \
                                __FUNCTION__, ##args);                                                                            \
  } while (0)

#define TRACE(fmt, args...)                                                                                                         \
  do {                                                                                                                              \
    if (ring_log::ins()->get_level() >= TRACE) {                                                                                    \
      ring_log::ins()->try_append("[TRACE]", "[%u]%s:%d(%s): " fmt "\n", gettid(), FILE_NAME_LOG_SPLIT(__FILE__).c_str(), __LINE__, \
                                  __FUNCTION__, ##args);                                                                            \
    }                                                                                                                               \
  } while (0)

#define DEBUG(fmt, args...)                                                                                                         \
  do {                                                                                                                              \
    if (ring_log::ins()->get_level() >= DEBUG) {                                                                                    \
      ring_log::ins()->try_append("[DEBUG]", "[%u]%s:%d(%s): " fmt "\n", gettid(), FILE_NAME_LOG_SPLIT(__FILE__).c_str(), __LINE__, \
                                  __FUNCTION__, ##args);                                                                            \
    }                                                                                                                               \
  } while (0)

#define INFO(fmt, args...)                                                                                                         \
  do {                                                                                                                             \
    if (ring_log::ins()->get_level() >= INFO) {                                                                                    \
      ring_log::ins()->try_append("[INFO]", "[%u]%s:%d(%s): " fmt "\n", gettid(), FILE_NAME_LOG_SPLIT(__FILE__).c_str(), __LINE__, \
                                  __FUNCTION__, ##args);                                                                           \
    }                                                                                                                              \
  } while (0)

#define NORMAL(fmt, args...)                                                                                                       \
  do {                                                                                                                             \
    if (ring_log::ins()->get_level() >= INFO) {                                                                                    \
      ring_log::ins()->try_append("[INFO]", "[%u]%s:%d(%s): " fmt "\n", gettid(), FILE_NAME_LOG_SPLIT(__FILE__).c_str(), __LINE__, \
                                  __FUNCTION__, ##args);                                                                           \
    }                                                                                                                              \
  } while (0)

#define WARN(fmt, args...)                                                                                                         \
  do {                                                                                                                             \
    if (ring_log::ins()->get_level() >= WARN) {                                                                                    \
      ring_log::ins()->try_append("[WARN]", "[%u]%s:%d(%s): " fmt "\n", gettid(), FILE_NAME_LOG_SPLIT(__FILE__).c_str(), __LINE__, \
                                  __FUNCTION__, ##args);                                                                           \
    }                                                                                                                              \
  } while (0)

#define ERROR(fmt, args...)                                                                                                         \
  do {                                                                                                                              \
    if (ring_log::ins()->get_level() >= ERROR) {                                                                                    \
      ring_log::ins()->try_append("[ERROR]", "[%u]%s:%d(%s): " fmt "\n", gettid(), FILE_NAME_LOG_SPLIT(__FILE__).c_str(), __LINE__, \
                                  __FUNCTION__, ##args);                                                                            \
    }                                                                                                                               \
  } while (0)

#define FATAL(fmt, args...)                                                                                                       \
  do {                                                                                                                            \
    ring_log::ins()->try_append("[FATAL]", "[%u]%s:%d(%s): " fmt "\n", gettid(), FILE_NAME_LOG_SPLIT(__FILE__).c_str(), __LINE__, \
                                __FUNCTION__, ##args);                                                                            \
  } while (0)

#define MSG_LOG_JSON(fmt, args...)                                                                                                  \
  do {                                                                                                                              \
    if (ring_log::ins()->get_level() >= ERROR) {                                                                                    \
      ring_log::ins()->try_append("[MSG]", "[%u]%s:%d(%s):\n " fmt "\n", gettid(), FILE_NAME_LOG_SPLIT(__FILE__).c_str(), __LINE__, \
                                  __FUNCTION__, ##args);                                                                            \
    }                                                                                                                               \
    fprintf(stdout, "\033[37m%s", ##args);                                                                                          \
    LINE_FEEDS                                                                                                                      \
  } while (0)

#define MSG_LOG(fmt, args...)                                                                                                     \
  do {                                                                                                                            \
    if (ring_log::ins()->get_level() >= ERROR) {                                                                                  \
      ring_log::ins()->try_append("[MSG]", "[%u]%s:%d(%s): " fmt "\n", gettid(), FILE_NAME_LOG_SPLIT(__FILE__).c_str(), __LINE__, \
                                  __FUNCTION__, ##args);                                                                          \
    }                                                                                                                             \
    fprintf(stdout, "\033[37m%s", ##args);                                                                                        \
    LINE_FEEDS                                                                                                                    \
  } while (0)

#define PURE_LOG(fmt, args...)                                   \
  do {                                                           \
    if (ring_log::ins()->get_level() >= ERROR) {                 \
      ring_log::ins()->try_append("[MSG]", "" fmt "\n", ##args); \
    }                                                            \
    fprintf(stdout, "\033[37m" fmt, ##args);                     \
    LINE_FEEDS                                                   \
  } while (0)

#define JsonPrint(jsonObject) MSG_LOG_JSON("%s", jsonObject.dump(4).c_str())
//*************************************************************************//

#endif
