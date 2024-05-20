#include "log.h"

namespace shbk2 {

Logger& Logger::Instance() {
  static Logger instance;
  return instance;
}

bool Logger::InitStdoutColorLogger(const std::string& name) {
  if (initialized_) return false;
  logger_ = spdlog::stdout_color_st(name);
  return initialized_ = true;
}

bool Logger::InitBasicFileLogger(const std::string& name,
                                 const std::string& filename) {
  if (initialized_) return false;
  logger_ = spdlog::basic_logger_st(name, filename);
  return initialized_ = true;
}

bool Logger::InitRotatingFileLogger(
    const std::string& name, const std::string& filename,
    size_t max_file_size, size_t max_files) {
  if (initialized_) return false;
  logger_ = spdlog::rotating_logger_st(name, filename, max_file_size,
                                       max_files);
  return initialized_ = true;
}

std::shared_ptr<spdlog::logger> Logger::logger() const {
  return initialized_ ? logger_ : nullptr;
}

bool Logger::SetLevel(Logger::Level level) const {
  if (!initialized_) return false;
  switch (level) {
    case Logger::Level::kDebug:logger_->set_level(spdlog::level::debug);
      break;
    case Logger::Level::kInfo:logger_->set_level(spdlog::level::info);
      break;
    case Logger::Level::kWarn:logger_->set_level(spdlog::level::warn);
      break;
    case Logger::Level::kErr:logger_->set_level(spdlog::level::err);
      break;
    case Logger::Level::kCritical:logger_->set_level(spdlog::level::critical);
  }
  return true;
}

}  // namespace shbk2