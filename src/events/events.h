#ifndef SHBK2_EVENTS_H_
#define SHBK2_EVENTS_H_

#include <cstdint>

#include "globals.h"
#include "shbk2.pb.h"
#include "session.h"

namespace shbk2 {

enum class EventType : std::uint16_t {
  kPhoneReq = 0x01,
  kPhoneRes = 0x02,
  kLoginReq = 0x03,
  kLoginRes = 0x04,
  kCloseSession = 0xfe,
  kUnknown = 0xff,
};

class Session;  // forward declaration
class IEvent {
 public:
  [[nodiscard]] virtual std::int32_t ByteSize() const = 0;
  virtual bool SerializeToArray(void* data, int size) const = 0;
  virtual bool ParseFromArray(const void* data, int size) = 0;
  [[nodiscard]] EventType type() const { return type_; }
  [[nodiscard]] std::uint32_t id() const { return id_; }
  [[nodiscard]] Session* session() const { return session_; }
  void set_session(Session* session) { session_ = session; }

  explicit IEvent(EventType type) : type_{type}, id_{sequence_++} {}
  virtual ~IEvent() = default;

 private:
  EventType type_;
  std::uint32_t id_;
  Session* session_{nullptr};
  static std::uint32_t sequence_;
};

class EvPhoneReq : public IEvent {
 public:
  std::int32_t ByteSize() const override { return phone_req_.ByteSize(); }
  bool SerializeToArray(void* data, int size) const override;
  bool ParseFromArray(const void* data, int size) override;
  const std::string& phone_number() const { return phone_req_.phone_number(); }

  EvPhoneReq() : IEvent{EventType::kPhoneReq} {}
  ~EvPhoneReq() override = default;

 private:
  PhoneReq phone_req_;
};

class EvPhoneRes : public IEvent {
 public:
  std::int32_t ByteSize() const override { return phone_res_.ByteSize(); }
  bool SerializeToArray(void* data, int size) const override;
  bool ParseFromArray(const void* data, int size) override;

  EvPhoneRes(std::int32_t identifier, ErrorCode code);
  ~EvPhoneRes() override = default;

 private:
  PhoneRes phone_res_;
};

class EvLoginReq : public IEvent {
 public:
  std::int32_t ByteSize() const override { return login_req_.ByteSize(); }
  bool SerializeToArray(void* data, int size) const override;
  bool ParseFromArray(const void* data, int size) override;
  const std::string& phone_number() const { return login_req_.phone_number(); }
  std::int32_t identifier() const { return login_req_.identifier(); }

  EvLoginReq() : IEvent{EventType::kLoginReq} {}
  ~EvLoginReq() override = default;

 private:
  LoginReq login_req_;
};

class EvLoginRes : public IEvent {
 public:
  std::int32_t ByteSize() const override { return login_res_.ByteSize(); }
  bool SerializeToArray(void* data, int size) const override;
  bool ParseFromArray(const void* data, int size) override;

  explicit EvLoginRes(ErrorCode code);
  ~EvLoginRes() override = default;

 private:
  LoginRes login_res_;
};

}  // namespace shbk2

#endif  // SHBK2_EVENTS_H_