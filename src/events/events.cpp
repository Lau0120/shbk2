#include "events.h"

namespace shbk2 {

std::uint32_t IEvent::sequence_ = 0;

bool EvPhoneReq::SerializeToArray(void* data, int size) const {
  return phone_req_.SerializeToArray(data, size);
}

bool EvPhoneReq::ParseFromArray(const void* data, int size) {
  return phone_req_.ParseFromArray(data, size);
}

bool EvPhoneRes::SerializeToArray(void* data, int size) const {
  return phone_res_.SerializeToArray(data, size);
}

bool EvPhoneRes::ParseFromArray(const void* data, int size) {
  return phone_res_.ParseFromArray(data, size);
}

EvPhoneRes::EvPhoneRes(std::int32_t identifier, ErrorCode code) :
    IEvent{EventType::kPhoneRes} {
  phone_res_.set_code(static_cast<std::int32_t>(code));
  phone_res_.set_desc(kErrorCodeDescMap.at(code));
  phone_res_.set_identifier(identifier);
}

bool EvLoginReq::SerializeToArray(void* data, int size) const {
  return login_req_.SerializeToArray(data, size);
}

bool EvLoginReq::ParseFromArray(const void* data, int size) {
  return login_req_.ParseFromArray(data, size);
}

bool EvLoginRes::SerializeToArray(void* data, int size) const {
  return login_res_.SerializeToArray(data, size);
}

bool EvLoginRes::ParseFromArray(const void* data, int size) {
  return login_res_.ParseFromArray(data, size);
}

EvLoginRes::EvLoginRes(ErrorCode code) : IEvent{EventType::kLoginRes} {
  login_res_.set_code(static_cast<std::int32_t>(code));
  login_res_.set_desc(kErrorCodeDescMap.at(code));
}

}  // namespace shbk2