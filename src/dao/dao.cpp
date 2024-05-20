#include "dao.h"

#include <memory>
#include <sstream>
#include <stdexcept>

#include "globals.h"

namespace shbk2 {

UserDAO& UserDAO::Instance() {
  static UserDAO instance;
  if (!instance.is_initialized_) {
    dbcp_mysql::ConnPool::DbInfo db_info;
    db_info.host = CONFIG.DatabaseGet<std::string>("host");
    db_info.user = CONFIG.DatabaseGet<std::string>("user");
    db_info.pwd = CONFIG.DatabaseGet<std::string>("pwd");
    db_info.db = CONFIG.DatabaseGet<std::string>("db");
    db_info.port = CONFIG.DatabaseGet<unsigned int>("port");
    int min_size = CONFIG.DatabaseGet<int>("min-size");
    int max_size = CONFIG.DatabaseGet<int>("max-size");
    instance.waiting_seconds_ = CONFIG.DatabaseGet<long long>("waiting-seconds");
    instance.pool_ = std::make_unique<dbcp_mysql::ConnPool>(
        db_info, min_size, max_size);
    instance.is_initialized_ = true;
  }
  return instance;
}

bool UserDAO::CreateTable() const {
  if (!is_initialized_) return false;
  std::stringstream ss;
  ss << "CREATE TABLE IF NOT EXISTS user (";
  ss << "user_id       INT(16)        NOT NULL AUTO_INCREMENT ,";
  ss << "user_phonenum VARCHAR(16)    NOT NULL DEFAULT '13000000000' ,";
  ss << "user_name     VARCHAR(128)   NOT NULL DEFAULT '' ,";
  ss << "user_register TIMESTAMP      NOT NULL DEFAULT CURRENT_TIMESTAMP ,";
  ss << "user_balance  DECIMAL(6, 2)  NOT NULL DEFAULT 0.0 ,";
  ss << "PRIMARY KEY (user_id)) ENGINE = InnoDB";
  auto conn_handler = pool_->RequestHandler(waiting_seconds_);
  if (conn_handler == nullptr) return false;
  try {
    auto sql_result = conn_handler->Execute(ss.str());
  } catch (const std::exception& e) {
    LOGGER->error("UserDAO::CreateTable() - {}", e.what());
    return false;
  }
  return true;
}

bool UserDAO::IsExist(const std::string& phone_number) const {
  if (!is_initialized_) return false;
  std::stringstream ss;
  ss << "SELECT * FROM user WHERE user_phonenum = '" << phone_number << "'";
  auto conn_handler = pool_->RequestHandler(waiting_seconds_);
  dbcp_mysql::ConnHandler::SqlResult sql_result;
  if (conn_handler == nullptr) return false;
  try {
    sql_result = conn_handler->Execute(ss.str());
  } catch (const std::exception& e) {
    LOGGER->error("UserDAO::IsExist() - {}", e.what());
    return false;
  }
  return sql_result.second != 0;
}

bool UserDAO::Insert(const std::string& phone_number) const {
  if (!is_initialized_) return false;
  std::stringstream ss;
  ss << "INSERT user (user_phonenum)";
  ss << "VALUES ('" << phone_number << "')";
  auto conn_handler = pool_->RequestHandler(waiting_seconds_);
  dbcp_mysql::ConnHandler::SqlResult sql_result;
  if (conn_handler == nullptr) return false;
  try {
    sql_result = conn_handler->Execute(ss.str());
  } catch (const std::exception& e) {
    LOGGER->error("UserDAO::Insert() - {}", e.what());
    return false;
  }
  return sql_result.second == 1;
}

}  // namespace shbk2