#include "dem.h"
#include <cstdint>

bool Dem::SetEventStatus(uint16_t event_id, DiagEventStatus event_status, const std::string &info) {
  if (event_id > test_config_.size()) {
    return false;
  }

  if (!CheckCurrentAndNewStatus(test_config_[event_id].event_status, event_status)) {
    return true;
  }

  test_config_[event_id].event_status = event_status;

  CounterDebounce(test_config_[event_id]);

  if (test_config_[event_id].event_status == kTestNocompleted) {
    test_config_[event_id].info = info;
    test_config_[event_id].fail_tested = false;
    test_config_[event_id].pass_tested = false;
  } else if (test_config_[event_id].event_status == kPass && !test_config_[event_id].pass_tested) {
    test_config_[event_id].info = info;
    test_config_[event_id].fail_tested = false;
    test_config_[event_id].pass_tested = true;
  } else if (test_config_[event_id].event_status == kFail && !test_config_[event_id].fail_tested) {
    test_config_[event_id].info = info;
    test_config_[event_id].pass_tested = false;
    test_config_[event_id].fail_tested = true;
  }

  return true;
}

bool Dem::ClearEventMask(uint64_t event_id_mask) {
  for (auto &config : test_config_) {
    if (event_id_mask == 0) {
      break;
    }
    if (event_id_mask & 1) {
      config.event_status = kTestNocompleted;
    }
    event_id_mask >>= 1;
  }

  return true;
}

uint32_t Dem::ReadPassEventMask() {
  uint32_t value = 0;
  uint8_t index = 0;
  for (auto &config : test_config_) {
    if (config.event_status == kPass) {
      value |= (0x0000001 << index);
    }
    index++;
  }

  return value;
}

uint32_t Dem::ReadFailEventMask() {
  uint32_t value = 0;
  uint8_t index = 0;
  for (auto &config : test_config_) {
    if (config.event_status == kFail) {
      value |= (0x0000001 << index);
    }
    index++;
  }

  return value;
}

void Dem::AddTestConfig(uint16_t event_id, uint16_t fail_para, uint16_t pass_para, DiagEventStatus status) {
  test_config_.push_back({event_id, fail_para, pass_para, status, 0, 0, 0, 0, 0, 0, ""});
}

std::vector<DiagTestConfig> &Dem::GetTestConfig() { return test_config_; }

const std::string &Dem::GetDiagInfo(uint16_t event_id) { return test_config_[event_id].info; }

void Dem::CounterDebounce(DiagTestConfig &config) {
  if (config.event_status == kTestNocompleted) {
    config.fail_counter = 0;
    config.pass_counter = 0;
  } else if (config.event_status == kPrePass) {
    config.fail_counter = 0;
    if (config.pass_counter < config.pass_para) {
      config.pass_counter++;
    }
    if (config.pass_counter >= config.pass_para) {
      config.event_status = kPass;
    }
  } else if (config.event_status == kPreFail) {
    config.pass_counter = 0;
    if (config.fail_counter < config.fail_para) {
      config.fail_counter++;
    }
    if (config.fail_counter >= config.fail_para) {
      config.event_status = kFail;
    }
  } else if (config.event_status == kPass) {
    config.fail_counter = 0;
  } else if (config.event_status == kFail) {
    config.pass_counter = 0;
  }
}

bool Dem::CheckCurrentAndNewStatus(DiagEventStatus current_status, DiagEventStatus new_status) {
  bool ret = true;
  if (current_status == kPass && (new_status == kPrePass || new_status == kPass)) {
    ret = false;
  } else if (current_status == kFail && (new_status == kPreFail || new_status == kFail)) {
    ret = false;
  }

  return ret;
}