#ifndef SHBK2_SYS_CONFIG_H_
#define SHBK2_SYS_CONFIG_H_

#include <string>

#include "toml11/toml.hpp"

namespace shbk2 {

class SysConfig {
 public:
  static SysConfig& Instance();

  void Load(const std::string& filename);
  template<typename T>
  T ServerGet(const std::string& key) const;
  template<typename T>
  T DatabaseGet(const std::string& key) const;
  [[nodiscard]] bool IsLoaded() const { return loaded_; }

 private:
  SysConfig() = default;

  toml::value config_;
  bool loaded_{false};
  toml::value server_table_;
  toml::value database_table_;
};

template<typename T>
T SysConfig::ServerGet(const std::string& key) const {
  return toml::find<T>(server_table_, key);
}

template<typename T>
T SysConfig::DatabaseGet(const std::string& key) const {
  return toml::find<T>(database_table_, key);
}

}  // namespace shbk2

#endif  // SHBK2_SYS_CONFIG_H_
