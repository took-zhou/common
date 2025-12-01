/* MIT License
 *
 * Copyright (c) 2018 Björn Blissing
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _FLAMEPROFILER_H_
#define _FLAMEPROFILER_H_

#if 0
#define USE_PROFILER
#endif

#ifndef USE_PROFILER
// Expand to nothing if profiling is disabled
#define PZone(name)
#define PZoneCat(name, category)
#define PMetadata(title, value)
#else
// Macro concatenation
#define CONCAT_IMPL(x, y) x##y
#define MACRO_CONCAT(x, y) CONCAT_IMPL(x, y)
// Macro names
#define PZone(name) profiler::Zone MACRO_CONCAT(profileZone, __COUNTER__)(name)
#define PZoneCat(name, category) profiler::Zone MACRO_CONCAT(profileZone, __COUNTER__)(name, category)
#define PMetadata(title, value) profiler::FlameGraphWriter::instance().addMetadata(title, value)
#endif  // USE_PROFILER

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <limits>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace profiler {

struct TracePoint {
  char name[64];
  char category[40];
  uint64_t time_start;
  uint64_t time_end;
  uint32_t process_id;
  uint32_t thread_id;
};  // 128 byte struct

class FlameGraphWriter {
 public:
  ~FlameGraphWriter() { WriteData(); }

  static FlameGraphWriter &Instance() {
    static FlameGraphWriter instance;
    return instance;
  }
  void AddMetadata(const std::string &title, const std::string &value) {
    if (m_write_flag_) {
      std::pair<std::string, std::string> metadata = std::make_pair(title, value);
      m_metadata_.push_back(metadata);
    }
  }
  void AddTracePoint(const TracePoint &point) {
    if (m_write_flag_) {
      m_tracepoints_.push_back(point);
    }
  }

  void StartAddData() { m_write_flag_ = true; }

  void StopAddData() {
    m_write_flag_ = false;
    WriteData();
  }

  void SetFilePath(const std::string &filedir) { m_filename_ = filedir + "/profile.json"; }

 private:
  FlameGraphWriter() {
    // Reserve points to avoid early memory reallocations
    m_tracepoints_.reserve(128);
  }
  void WriteData() {
    // Open file
    std::ofstream ofs;
    ofs.open(m_filename_, std::ofstream::out);

    // Write content
    if (ofs.is_open()) {
      // Header
      ofs << "{\n";
      ofs << "\t\"traceEvents\": ";
      // Begin trace events
      ofs << "[";
      // For each tracepoint write
      std::string separator = "";

      for (const auto &tracepoint : m_tracepoints_) {
        uint64_t duration = tracepoint.time_end - tracepoint.time_start;
        ofs << separator << "\n";
        ofs << "\t\t{";
        ofs << " \"pid\":" << tracepoint.process_id << ",";
        ofs << " \"tid\":" << tracepoint.thread_id << ",";
        ofs << " \"ts\":" << tracepoint.time_start << ",";
        ofs << " \"dur\":" << duration << ",";
        ofs << " \"ph\":\"X\",";
        ofs << " \"name\":\"" << tracepoint.name << "\",";
        ofs << " \"cat\":\"" << tracepoint.category << "\"";
        ofs << "}";
        separator = ",";
      }

      ofs << "\n\t]";
      // End trace events

      // Begin metadata
      for (const auto &metadata : m_metadata_) {
        ofs << ",\n";
        ofs << "\t\"" << metadata.first << "\": \"" << metadata.second << "\"";
      }

      // End metadata
      ofs << "\n}\n";
      // Close file
      ofs.close();
    }

    m_metadata_.clear();
    m_tracepoints_.clear();
  }
  FlameGraphWriter(const FlameGraphWriter &) = delete;             // No copy allowed
  FlameGraphWriter(const FlameGraphWriter &&) = delete;            // No move allowed
  FlameGraphWriter &operator=(const FlameGraphWriter &) = delete;  // No assignment allowed
  FlameGraphWriter &operator=(FlameGraphWriter &&) = delete;       // No move assignment allowed

  bool m_write_flag_ = false;
  std::string m_filename_ = "profile.json";
  std::vector<TracePoint> m_tracepoints_;
  std::vector<std::pair<std::string, std::string> > m_metadata_;
};

class Zone {
 public:
  Zone(const char *name, const char *category = "default") {
    snprintf(m_tracepoint_.name, sizeof(m_tracepoint_.name), "%s", name);
    snprintf(m_tracepoint_.category, sizeof(m_tracepoint_.category), "%s", category);
    size_t thread = std::hash<std::thread::id>()(std::this_thread::get_id());
    m_tracepoint_.thread_id = static_cast<uint32_t>(thread);
    auto timer = std::chrono::high_resolution_clock::now();
    m_tracepoint_.time_start = std::chrono::time_point_cast<std::chrono::microseconds>(timer).time_since_epoch().count();
#ifdef _WIN32
    m_tracepoint.processId = static_cast<uint32_t>(GetCurrentProcessId());
#else
    m_tracepoint_.process_id = static_cast<uint32_t>(::getpid());
#endif
  }

  ~Zone() {
    auto timer = std::chrono::high_resolution_clock::now();
    m_tracepoint_.time_end = std::chrono::time_point_cast<std::chrono::microseconds>(timer).time_since_epoch().count();
    FlameGraphWriter::Instance().AddTracePoint(m_tracepoint_);
  }

 private:
  TracePoint m_tracepoint_;
};
}  // namespace profiler

#endif  // !_FLAMEPROFILER_H_