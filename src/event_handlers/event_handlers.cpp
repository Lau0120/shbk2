#include "event_handlers.h"

#include <cstdlib>
#include <ctime>

#include "dao.h"

namespace shbk2 {

std::shared_ptr<void> PhoneReqEventHandler::Handle(
    std::shared_ptr<IEvent> event) {
  auto phone_req_event = std::static_pointer_cast<EvPhoneReq>(event);
  std::int32_t identifier = generate_identifier();
  phone_req_event->session()->set_identifier(identifier);
  phone_req_event->session()->set_phone_number(phone_req_event->phone_number());
  LOGGER->info(
      "PhoneReqEventHandler::Handle() - session({}) was handled ({}, {})",
      phone_req_event->session()->socket(),
      phone_req_event->phone_number(), identifier);
  auto result = std::make_shared<EvPhoneRes>(identifier, ErrorCode::kSuccess);
  result->set_session(phone_req_event->session());
  return result;
}

std::int32_t PhoneReqEventHandler::generate_identifier() {
  std::srand(static_cast<std::uint32_t>(std::time(nullptr)));
  return static_cast<std::int32_t>(std::rand() % (999999 - 100000) + 100000);
}

std::shared_ptr<void> LoginReqEventHandler::Handle(
    std::shared_ptr<IEvent> event) {
  auto login_req_event = std::static_pointer_cast<EvLoginReq>(event);
  std::shared_ptr<IEvent> result;
  if (login_req_event->session()->phone_number() !=
          login_req_event->phone_number() ||
      login_req_event->session()->identifier() !=
          login_req_event->identifier()) {
    LOGGER->warn(
        "LoginReqEventHandler::Handle() - session({}) was handled ({}, {})",
        login_req_event->session()->socket(),
        login_req_event->phone_number(), login_req_event->identifier());
    result = std::make_shared<EvLoginRes>(ErrorCode::kInvalidData);
  } else {
    if (!UserDAO::Instance().IsExist(login_req_event->phone_number())) {
      if (!UserDAO::Instance().Insert(login_req_event->phone_number())) {
        LOGGER->error(
            "LoginReqEventHandler::Handle() - session({}) was handled ({}, {})",
            login_req_event->session()->socket(),
            login_req_event->phone_number(), login_req_event->identifier());
        result = std::make_shared<EvLoginRes>(ErrorCode::kProcessFailed);
      }
    }
    LOGGER->info(
        "LoginReqEventHandler::Handle() - session({}) was handled ({}, {})",
        login_req_event->session()->socket(),
        login_req_event->phone_number(), login_req_event->identifier());
    result = std::make_shared<EvLoginRes>(ErrorCode::kSuccess);
  }
  result->set_session(login_req_event->session());
  return result;
}

}  // namespace shbk2