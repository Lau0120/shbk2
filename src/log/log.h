#ifndef SHBK2_LOG_H_
#define SHBK2_LOG_H_

#include <memory>
#include <string>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"

namespace shbk2 {

class Logger {
 public:
  enum class Level {
    kDebug,
    kInfo,
    kWarn,
    kErr,
    kCritical,
  };

  static Logger& Instance();

  bool InitStdoutColorLogger(const std::string& name);
  bool InitBasicFileLogger(const std::string& name,
                           const std::string& filename);
  bool InitRotatingFileLogger(const std::string& name,
                              const std::string& filename,
                              size_t max_file_size, size_t max_files);
  [[nodiscard]] std::shared_ptr<spdlog::logger> logger() const;
  [[nodiscard]] bool SetLevel(Logger::Level level) const;
  [[nodiscard]] bool IsInitialized() const { return initialized_; }

 private:
  Logger() = default;

  std::shared_ptr<spdlog::logger> logger_;
  bool initialized_{false};
};

}  // namespace shbk2

#endif  // SHBK2_LOG_H_