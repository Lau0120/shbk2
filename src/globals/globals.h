#ifndef SHBK2_GLOBALS_H_
#define SHBK2_GLOBALS_H_

#include <cstdint>
#include <unordered_map>

#include "log.h"
#include "sys_config.h"

namespace shbk2 {

#define LOGGER  (shbk2::Logger::Instance().logger())
#define CONFIG  (shbk2::SysConfig::Instance())

enum class ErrorCode : std::int32_t {
  kSuccess = 200,
  kInvalidData = 400,
  kMethodNotAllowed = 401,
  kProcessFailed = 402,
};

const std::unordered_map<ErrorCode, std::string> kErrorCodeDescMap{
    {ErrorCode::kSuccess, "ok."},
    {ErrorCode::kInvalidData, "invalid data."},
    {ErrorCode::kMethodNotAllowed, "method not allowed."},
    {ErrorCode::kProcessFailed, "Process failed."},};

}  // namespace shbk2

#endif  // SHBK2_GLOBALS_H_