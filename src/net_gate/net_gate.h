#ifndef SHBK2_NET_GATE_H_
#define SHBK2_NET_GATE_H_

#include <string>
#include <memory>
#include <cstdint>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include <event2/bufferevent.h>
#include <event2/event.h>
#include <event2/util.h>
#include <event2/buffer.h>
#include <event2/listener.h>

#include "globals.h"
#include "session.h"
#include "msg_proxy.h"
#include "des.h"

namespace shbk2 {

class NetGate {
 public:
  NetGate(const std::string& ip, std::uint16_t port);

  bool Open(std::int32_t backlog);
  void Dispatch();

 private:
  static void FreeEventBase(event_base* evBase) { event_base_free(evBase); }
  static void FreeEvconnlistener(evconnlistener* evConnListener);
  static void ListenerCallback(evconnlistener* pListener, evutil_socket_t sock,
                               sockaddr* addr, int addr_len, void* data);
  static void ReqCallback(bufferevent* bufev, void* ctx);
  static void ResCallback(bufferevent* bufev, void* ctx);
  static void ErrCallback(bufferevent* bufev, std::int16_t events, void* ctx);

  void handle_results();

  std::unique_ptr<event_base, decltype(NetGate::FreeEventBase)*> ev_base_;
  sockaddr_in server_addr_{};
  std::unique_ptr<evconnlistener,
                  decltype(NetGate::FreeEvconnlistener)*> ev_listener_;
};

}  // namespace shbk2

#endif  // SHBK2_NET_GATE_H_