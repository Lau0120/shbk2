#ifndef SHBK2_SESSION_H_
#define SHBK2_SESSION_H_

#include <cstdint>
#include <string>
#include <memory>
#include <iostream>
#include <utility>

#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include "globals.h"
#include "events.h"

namespace shbk2 {

class Buffer {
 public:
  Buffer() : total_{0}, last_{0}, buf_{nullptr} {}
  ~Buffer() { Reset(); }

  [[nodiscard]] std::uint32_t total() const { return total_; }
  [[nodiscard]] std::uint32_t last() const { return last_; }
  [[nodiscard]] std::shared_ptr<char> buffer() const { return buf_; }
  void IncrementLast(std::uint32_t num) { last_ += num; }
  bool Alloc(std::uint32_t size);
  void Reset();
  void Realloc(std::uint32_t size);
  [[nodiscard]] bool IsEnough() const { return last() == total(); }

 private:
  std::uint32_t total_;
  std::uint32_t last_;
  std::shared_ptr<char> buf_;
};

enum class EventType : std::uint16_t;  // forward declaration
class Session {
 public:
  Session(std::int32_t socket, std::string ip, std::uint16_t port) :
      socket_{socket}, ip_{std::move(ip)}, port_{port},
      count_{0}, bufev_{nullptr},
      stat_{Session::State::WAITING_REQUEST},
      identifier_{0}, is_login_{false} {}
  Session(const Session& other) = delete;
  Session& operator=(const Session& other) = delete;

 public:
  enum class State {
    WAITING_REQUEST,
    PARSING_HEAD,
    PARSING_BODY,
    HANDLING,
  };

 public:
  [[nodiscard]] std::int32_t socket() const { return socket_; }
  [[nodiscard]] const std::string& ip() const { return ip_; }
  [[nodiscard]] std::uint16_t port() const { return port_; }
  [[nodiscard]] std::uint8_t count() const { return count_; }
  void IncrementCount() { ++count_; }
  [[nodiscard]] std::shared_ptr<bufferevent> bufev() const { return bufev_; }
  void set_bufev(bufferevent* bufev);
  Buffer& req_buf() { return req_buf_; }
  [[nodiscard]] Session::State state() const { return stat_; }
  void set_state(Session::State stat) { stat_ = stat; }
  [[nodiscard]] EventType event_type() const { return event_type_; }
  void set_event_type(EventType type) { event_type_ = type; }
  [[nodiscard]] std::int32_t identifier() const { return identifier_; }
  void set_identifier(std::int32_t identifier) { identifier_ = identifier; }
  void set_phone_number(const std::string& phone_number);
  [[nodiscard]] const std::string& phone_number() const;

 private:
  static void free_bufferevent(bufferevent* bufev) { bufferevent_free(bufev); }

  std::int32_t socket_;
  std::string ip_;
  std::uint16_t port_;
  std::uint8_t count_;
  std::shared_ptr<bufferevent> bufev_;
  EventType event_type_;
  Buffer req_buf_;
  Session::State stat_;
  std::int32_t identifier_;
  std::string phone_number_;
  bool is_login_;
};

}

#endif  // SHBK2_SESSION_H_