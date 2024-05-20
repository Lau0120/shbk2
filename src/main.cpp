#include <cstdint>
#include <iostream>
#include <chrono>
#include <thread>

#include "globals.h"
#include "net_gate.h"
#include "des.h"
#include "event_handlers.h"
#include "dao.h"

using namespace std;
using namespace shbk2;

int main(int argc, char* argv[]) {
  if (argc != 2) {
    cerr << "usage: program <config path>" << endl;
    exit(1);
  }

  SysConfig::Instance().Load(argv[1]);
  if (!SysConfig::Instance().IsLoaded()) {
    cerr << "load config error" << endl;
    exit(2);
  }

  Logger::Instance().InitStdoutColorLogger("default");
  if (!Logger::Instance().IsInitialized()) {
    cerr << "initialize logger error" << endl;
    exit(3);
  } else if (!Logger::Instance().SetLevel(Logger::Level::kDebug)) {
    cerr << "set level of uninitialized logger" << endl;
  }

  if (!UserDAO::Instance().CreateTable()) {
    LOGGER->error("create user table error");
    exit(4);
  }

  DispatchEventsService::Instance().RegisterHandler(
      EventType::kPhoneReq, std::make_shared<PhoneReqEventHandler>());
  DispatchEventsService::Instance().RegisterHandler(
      EventType::kLoginReq, std::make_shared<LoginReqEventHandler>());

  NetGate ng(CONFIG.ServerGet<std::string>("ip"),
             CONFIG.ServerGet<std::uint16_t>("port"));
  ng.Open(CONFIG.ServerGet<std::int32_t>("backlog"));
  while (true) {
    ng.Dispatch();
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return 0;
}
