#include "msg_proxy.h"

#include <cstring>
#include <exception>
#include <stdexcept>
#include <iostream>

namespace shbk2 {

MsgProxy& MsgProxy::Instance() {
  static MsgProxy instance;
  return instance;
}

std::shared_ptr<IEvent> MsgProxy::Pack(Session* session) {
  if (session->state() == Session::State::HANDLING)
    throw std::runtime_error("invalid session state");

  if (session->state() == Session::State::WAITING_REQUEST) {
    session->set_state(Session::State::PARSING_HEAD);
    session->req_buf().Alloc(HEADER_SIZE);
  }

  if (session->state() == Session::State::PARSING_HEAD) {
    std::uint32_t curt = bufferevent_read(
        session->bufev().get(),
        session->req_buf().buffer().get() + session->req_buf().last(),
        session->req_buf().total() - session->req_buf().last());
    session->req_buf().IncrementLast(curt);
    if (session->req_buf().IsEnough()) {
      if (std::strncmp(session->req_buf().buffer().get(),
                       HEADER_CONT.c_str(), 4) != 0) {
        throw std::invalid_argument("unknown identifier");
      }
      EventType event_type = static_cast<EventType>(
          *(reinterpret_cast<std::uint16_t*>(
              session->req_buf().buffer().get() + HEADER_CONT.size())));
      std::uint32_t msg_size = *(reinterpret_cast<std::uint32_t*>(
          session->req_buf().buffer().get() +
          HEADER_CONT.size() + sizeof(std::int16_t)));
      session->set_event_type(event_type);
      session->set_state(Session::State::PARSING_BODY);
      session->req_buf().Realloc(msg_size);
    }
  }

  if (session->state() == Session::State::PARSING_BODY &&
      evbuffer_get_length(bufferevent_get_input(session->bufev().get())) > 0) {
    std::uint32_t curt = bufferevent_read(
        session->bufev().get(),
        session->req_buf().buffer().get() + session->req_buf().last(),
        session->req_buf().total() - session->req_buf().last());
    session->req_buf().IncrementLast(curt);
    if (session->req_buf().IsEnough()) {
      std::shared_ptr<IEvent> event;
      switch (session->event_type()) {
        case EventType::kPhoneReq:
          event = std::make_shared<EvPhoneReq>();
          break;
        case EventType::kLoginReq:
          event = std::make_shared<EvLoginReq>();
          break;
        default:
          throw std::invalid_argument("unknown event type");
      }
      event->set_session(session);
      event->ParseFromArray(session->req_buf().buffer().get(),
                            session->req_buf().total());
      session->req_buf().Reset();
      return event;
    }
  }

  return nullptr;
}

std::pair<std::shared_ptr<void>, std::uint32_t> MsgProxy::Unpack(
    const std::shared_ptr<IEvent>& event) {
  LOGGER->info("MsgProxy::Unpack() - unpacking message in session({})",
               event->session()->socket());
  std::uint32_t msg_size = HEADER_SIZE + event->ByteSize();
  auto msg = std::shared_ptr<void>(new char[msg_size]);
  char* p = static_cast<char*>(msg.get());
  memcpy(p, HEADER_CONT.c_str(), HEADER_CONT.size());
  p += HEADER_CONT.size();
  *(reinterpret_cast<std::uint16_t*>(p)) =
      static_cast<std::uint16_t>(event->type());
  p += sizeof(event->type());
  *(reinterpret_cast<std::uint32_t*>(p)) = event->ByteSize();
  p += sizeof(event->ByteSize());
  event->SerializeToArray(p, event->ByteSize());
  LOGGER->info("MsgProxy::Unpack() - unpacking message in session({}) complete",
               event->session()->socket());
  return {msg, msg_size};
}

}