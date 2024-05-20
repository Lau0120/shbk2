#include "net_gate.h"

namespace shbk2 {

NetGate::NetGate(const std::string& ip, std::uint16_t port) :
    ev_base_{event_base_new(), NetGate::FreeEventBase},
    ev_listener_{nullptr, nullptr} {
  server_addr_.sin_family = AF_INET;
  server_addr_.sin_addr.s_addr = inet_addr(ip.c_str());
  server_addr_.sin_port = htons(port);
}

bool NetGate::Open(std::int32_t backlog) {
  ev_listener_ = std::unique_ptr<
      evconnlistener, decltype(NetGate::FreeEvconnlistener)*>(
      evconnlistener_new_bind(
          ev_base_.get(), ListenerCallback, static_cast<void*>(ev_base_.get()),
          LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, backlog,
          reinterpret_cast<sockaddr*>(&server_addr_), sizeof(server_addr_)),
      NetGate::FreeEvconnlistener);
  LOGGER->info("NetGate::Open() - server start listening");
  return ev_listener_ != nullptr;
}

void NetGate::Dispatch() {
  event_base_loop(ev_base_.get(), EVLOOP_NONBLOCK);
  handle_results();
}

void NetGate::FreeEvconnlistener(evconnlistener* evConnListener) {
  evconnlistener_free(evConnListener);
}

void NetGate::ListenerCallback(evconnlistener* pListener, evutil_socket_t sock,
                               sockaddr* addr, int addr_len, void* data) {
  auto client_addr = reinterpret_cast<sockaddr_in*>(addr);
  auto ev_base = static_cast<event_base*>(data);
  evutil_make_socket_nonblocking(sock);

  auto* session = new Session(sock, inet_ntoa(client_addr->sin_addr),
                              ntohs(client_addr->sin_port));
  LOGGER->info("NetGate::ListenerCallback() - accept client({}) {}:{}",
               session->socket(), session->ip(), session->port());

  session->set_bufev(bufferevent_socket_new(
      ev_base, session->socket(), BEV_OPT_CLOSE_ON_FREE));
  bufferevent_setcb(session->bufev().get(), NetGate::ReqCallback,
                    NetGate::ResCallback, NetGate::ErrCallback,
                    static_cast<void*>(session));
  bufferevent_enable(session->bufev().get(), EV_READ | EV_PERSIST);
  LOGGER->info("NetGate::ListenerCallback() -"
               "ready to receive data from session({})",
               session->socket());
}

void NetGate::ReqCallback(bufferevent* bufev, void* ctx) {
  auto session = static_cast<Session*>(ctx);
  std::shared_ptr<IEvent> event;
  try {
    event = MsgProxy::Instance().Pack(session);
  } catch (const std::exception& e) {
    LOGGER->error("NetGate::ReqCallback() -"
                  "parse message in session({}) error for {}",
                  session->socket(), e.what());
    delete session;
  }
  if (event == nullptr) {
    LOGGER->warn("NetGate::ReqCallback() -"
                 "message in session({}) was not packed yet",
                 session->socket());
    return;
  }
  if (!DispatchEventsService::Instance().Push(event)) {
    LOGGER->error("NetGate::ReqCallback() -"
                  "message in session({}) failed to Push",
                  session->socket());
    delete session;
  }
  session->set_state(Session::State::HANDLING);
  LOGGER->info("NetGate::ReqCallback() -"
               "message in session({}) was packed and pushed",
               session->socket());
}

void NetGate::ResCallback(bufferevent* bufev, void* ctx) {
  auto session = static_cast<Session*>(ctx);
  session->IncrementCount();
  LOGGER->info("NetGate::ResCallback() -"
               "complete session({}) {} time(s)\n",
               session->socket(), session->count());
}

void NetGate::ErrCallback(bufferevent* bufev, std::int16_t events, void* ctx) {
  auto session = static_cast<Session*>(ctx);
  if (events & BEV_EVENT_EOF) {
    LOGGER->info("NetGate::ErrCallback() - session({}) was closed",
                 session->socket());
  } else if (events & BEV_EVENT_ERROR) {
    LOGGER->error("NetGate::ErrCallback() - session({}) other error occurs",
                  session->socket());
  } else if ((events & BEV_EVENT_TIMEOUT) && (events & BEV_EVENT_READING)) {
    LOGGER->error("NetGate::ErrCallback() - session({}) reading timeout",
                  session->socket());
  } else if ((events & BEV_EVENT_TIMEOUT) && (events & BEV_EVENT_WRITING)) {
    LOGGER->error("NetGate::ErrCallback() - session({}) writing timeout",
                  session->socket());
  }
  delete session;
}

void NetGate::handle_results() {
  auto results = DispatchEventsService::Instance().Drag();
  for (const auto& result : results) {
    auto event = std::static_pointer_cast<IEvent>(result);
    auto package = MsgProxy::Instance().Unpack(event);
    bufferevent_write(event->session()->bufev().get(),
                      package.first.get(), package.second);
    event->session()->set_state(Session::State::WAITING_REQUEST);
    LOGGER->info("NetGate::handle_results() - event of session({}) done",
                 event->session()->socket());
  }
}

}  // namespace shbk2