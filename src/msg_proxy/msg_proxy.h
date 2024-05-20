#ifndef SHBK2_MSG_PROXY_H_
#define SHBK2_MSG_PROXY_H_

#include <cstdint>
#include <memory>
#include <string>

#include "globals.h"
#include "session.h"
#include "events.h"

namespace shbk2 {

class MsgProxy {
 public:
  static MsgProxy& Instance();

  std::shared_ptr<IEvent> Pack(Session* session);
  std::pair<std::shared_ptr<void>, std::uint32_t> Unpack(
      const std::shared_ptr<IEvent>& event);

 private:
  MsgProxy() = default;

  const std::string HEADER_CONT{"SHBK"};
  const std::uint32_t HEADER_SIZE{10};
};

}  // namespace shbk2

#endif  // SHBK2_MSG_PROXY_H_