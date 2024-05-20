#ifndef DBCP_MYSQL_H_
#define DBCP_MYSQL_H_

#include <mysql/mysql.h>

#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace dbcp_mysql {

/// @brief Represents the result of a SELECT query.
class SelectResult {
 public:
  explicit SelectResult(MYSQL_RES* result);
  ~SelectResult() { if (result_ != nullptr) mysql_free_result(result_); }

  std::vector<MYSQL_ROW> rows() const { return rows_; }
  std::vector<std::string> fields_name() const { return fields_name_; }

 private:
  MYSQL_RES* result_;
  std::vector<MYSQL_ROW> rows_;
  std::vector<std::string> fields_name_;
};

class ConnHandler;
/// @brief Represents a connection pool.
class ConnPool {
 public:
  friend class ConnHandler;

  /// @brief Struct to hold database connection information.
  struct DbInfo {
    std::string host;
    std::string user;
    std::string pwd;
    std::string db;
    unsigned int port;
  };

  /// @brief Default minimum connection count.
  static const std::uint32_t kDefaultMinConnCount{4};
  /// @brief Default maximum connection count.
  static const std::uint32_t kDefaultMaxConnCount{8};

  explicit ConnPool(const ConnPool::DbInfo& db_info,
                    std::uint32_t min_conn_count = kDefaultMinConnCount,
                    std::uint32_t max_conn_count = kDefaultMaxConnCount);
  ~ConnPool() { mysql_library_end(); }

  /// @brief Requests a connection handler from the pool.
  /// @param waiting_seconds Number of seconds to wait for a handler if the pool
  /// is empty.
  /// @return Unique pointer to a ConnHandler object.
  std::unique_ptr<ConnHandler> RequestHandler(long long waiting_seconds);

 private:
  /// @brief Wrapper class for MySQL connection handler.
  class HandlerWrapper {
   public:
    explicit HandlerWrapper(const ConnPool::DbInfo& db_info);
    ~HandlerWrapper() { if (handler_ != nullptr) mysql_close(handler_); }

    bool is_using() const { return is_using_; }
    void set_is_using(bool val) { is_using_ = val; }
    MYSQL* handler() const { return handler_; }

   private:
    MYSQL* handler_;
    bool is_using_;
  };

  /// @brief Allocates a connection handler from the pool.
  /// @param handler_wrapper Reference to a HandlerWrapper object.
  /// @param idx Index of the handler in the pool.
  /// @return Unique pointer to a ConnHandler object.
  std::unique_ptr<ConnHandler> AllocateConnHandler(
      const std::shared_ptr<HandlerWrapper>& handler_wrapper, size_t idx);
  /// @brief Releases a connection handler back to the pool.
  /// @param idx Index of the handler in the pool.
  void ReleaseConnHandler(size_t idx);

  const std::uint32_t kMinCount;
  const std::uint32_t kMaxCount;
  DbInfo db_info_;
  int using_handler_count_{0};
  std::vector<std::shared_ptr<HandlerWrapper>> handler_wrappers_;
  std::mutex handler_wrappers_mtx_;
  std::condition_variable handler_wrappers_cond_;
};

/// @brief Represents a connection handler.
class ConnHandler {
 public:
  using SqlResult = std::pair<std::shared_ptr<SelectResult>, std::uint64_t>;

  ConnHandler(MYSQL* handler, ConnPool* pool, size_t idx)
      : handler_{handler}, pool_{pool}, index_{idx} {}
  ~ConnHandler() { pool_->ReleaseConnHandler(index_); }

  SqlResult Execute(const std::string& sql) const;

 private:
  MYSQL* handler_;
  ConnPool* pool_;
  size_t index_;
};

}  // namespace dbcp_mysql

#endif  // DBCP_MYSQL_H_