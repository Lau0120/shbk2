#include "dbcp_mysql.h"

namespace dbcp_mysql {

SelectResult::SelectResult(MYSQL_RES* result) : result_{result} {
  MYSQL_ROW row = nullptr;
  while ((row = mysql_fetch_row(result_)) != nullptr) {
    rows_.push_back(row);
  }
  MYSQL_FIELD* field = nullptr;
  while ((field = mysql_fetch_field(result_)) != nullptr) {
    fields_name_.emplace_back(field->name);
  }
}

ConnPool::ConnPool(const ConnPool::DbInfo& db_info,
                   std::uint32_t min_conn_count, std::uint32_t max_conn_count)
    : db_info_{db_info}, kMinCount{min_conn_count}, kMaxCount{max_conn_count} {
  mysql_library_init(0, nullptr, nullptr);
  for (std::uint32_t i = 0; i < kMinCount; ++i) {
    handler_wrappers_.push_back(std::make_shared<HandlerWrapper>(db_info_));
  }
}

std::unique_ptr<ConnHandler> ConnPool::RequestHandler(
    long long waiting_seconds) {
  std::unique_lock<std::mutex> handler_wrappers_lock{handler_wrappers_mtx_};
  bool is_cond_satisfied = handler_wrappers_cond_.wait_for(
      handler_wrappers_lock, std::chrono::seconds{waiting_seconds},
      [this]() {
        return handler_wrappers_.size() != using_handler_count_ ||
            handler_wrappers_.size() < kMaxCount;
      });
  if (!is_cond_satisfied) return nullptr;

  for (size_t i = 0; i < handler_wrappers_.size(); ++i) {
    auto handler_wrapper = handler_wrappers_[i];
    if (!handler_wrapper->is_using()) {
      auto conn_handler = AllocateConnHandler(handler_wrapper, i);
      handler_wrappers_lock.unlock();
      return conn_handler;
    }
  }
  if (handler_wrappers_.size() < kMaxCount) {
    handler_wrappers_.push_back(std::make_shared<HandlerWrapper>(db_info_));
    auto conn_handler = AllocateConnHandler(
        handler_wrappers_.back(), handler_wrappers_.size() - 1);
    handler_wrappers_lock.unlock();
    return conn_handler;
  }

  return nullptr;
}

ConnPool::HandlerWrapper::HandlerWrapper(const ConnPool::DbInfo& db_info)
    : is_using_{false}, handler_{mysql_init(nullptr)} {
  handler_ = mysql_real_connect(
      handler_, db_info.host.c_str(), db_info.user.c_str(),
      db_info.pwd.c_str(), db_info.db.c_str(), db_info.port, nullptr, 0);
}

std::unique_ptr<ConnHandler> ConnPool::AllocateConnHandler(
    const std::shared_ptr<HandlerWrapper>& handler_wrapper, size_t idx) {
  std::unique_ptr<ConnHandler> conn_handler{
      new ConnHandler{handler_wrapper->handler(), this, idx}};
  ++using_handler_count_;
  handler_wrapper->set_is_using(true);
  return conn_handler;
}

void ConnPool::ReleaseConnHandler(size_t idx) {
  std::lock_guard<std::mutex> handler_wrappers_guard{handler_wrappers_mtx_};
  auto handler_wrapper = handler_wrappers_[idx];
  --using_handler_count_;
  handler_wrapper->set_is_using(false);
  handler_wrappers_cond_.notify_one();
}

ConnHandler::SqlResult ConnHandler::Execute(const std::string& sql) const {
  if (handler_ == nullptr) {
    throw std::runtime_error("handler is invalid...");
  }
  if (mysql_real_query(handler_, sql.c_str(), sql.length()) != 0) {
    throw std::runtime_error(mysql_error(handler_));
  }
  MYSQL_RES* result = mysql_store_result(handler_);
  return {result == nullptr ? nullptr : std::make_shared<SelectResult>(result),
      mysql_affected_rows(handler_)};
}

}  // namespace dbcp_mysql