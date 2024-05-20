#ifndef SHBK2_EVENT_HANDLERS_H_
#define SHBK2_EVENT_HANDLERS_H_

#include <memory>
#include <cstdint>

#include "globals.h"
#include "events.h"

namespace shbk2 {

class IEventHandler {
 public:
  virtual std::shared_ptr<void> Handle(std::shared_ptr<IEvent> event) = 0;
};

class PhoneReqEventHandler : public IEventHandler {
 public:
  std::shared_ptr<void> Handle(std::shared_ptr<IEvent> event) override;
 private:
  static std::int32_t generate_identifier();
};

class LoginReqEventHandler : public IEventHandler {
 public:
  std::shared_ptr<void> Handle(std::shared_ptr<IEvent> event) override;
};

}  // namespace shbk2

#endif  // SHBK2_EVENT_HANDLERS_H_