#ifndef SHBK2_SERVICES_H_
#define SHBK2_SERVICES_H_

#include <memory>

#include "dbcp_mysql.h"

namespace shbk2 {

class UserDAO {
 public:
  static UserDAO& Instance();

  [[nodiscard]] bool CreateTable() const;
  [[nodiscard]] bool IsExist(const std::string& phone_number) const;
  [[nodiscard]] bool Insert(const std::string& phone_number) const;

 private:
  UserDAO() = default;

  std::unique_ptr<dbcp_mysql::ConnPool> pool_{nullptr};
  bool is_initialized_{false};
  long long waiting_seconds_{0};
};

}  // namespace shbk2

#endif  // SHBK2_SERVICES_H_