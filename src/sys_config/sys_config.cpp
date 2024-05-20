#include "sys_config.h"

namespace shbk2 {

SysConfig& SysConfig::Instance() {
  static SysConfig instance;
  return instance;
}

void SysConfig::Load(const std::string& filename) {
  if (loaded_) return;
  config_ = toml::parse(filename);
  server_table_ = toml::find(config_, "server");
  database_table_ = toml::find(config_, "database");
  loaded_ = true;
}

}  // namespace shbk2