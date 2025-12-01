#ifndef DEM_H
#define DEM_H

#include <cstdint>
#include <string>
#include <vector>

enum DiagEventStatus { kTestNocompleted = 0, kPrePass = 1, kPass = 2, kPreFail = 3, kFail = 4 };

struct DiagTestConfig {
  uint16_t event_id;
  uint16_t fail_para;
  uint16_t pass_para;
  DiagEventStatus event_status;
  uint16_t fail_counter;
  uint16_t pass_counter;
  bool fail_tested;
  bool pass_tested;
  bool fail_recorded;
  bool pass_recorded;
  std::string info = "";
};

struct Dem {
 public:
  bool SetEventStatus(uint16_t event_id, DiagEventStatus event_status, const std::string &info = "");
  bool ClearEventMask(uint64_t event_id_mask);
  uint32_t ReadPassEventMask();
  uint32_t ReadFailEventMask();
  void AddTestConfig(uint16_t event_id, uint16_t fail_para, uint16_t pass_para, DiagEventStatus status);
  std::vector<DiagTestConfig> &GetTestConfig();
  const std::string &GetDiagInfo(uint16_t event_id);

 private:
  void CounterDebounce(DiagTestConfig &config);
  bool CheckCurrentAndNewStatus(DiagEventStatus current_status, DiagEventStatus new_status);
  std::vector<DiagTestConfig> test_config_;
};

#endif