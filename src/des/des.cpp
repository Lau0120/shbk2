#include "des.h"

namespace shbk2 {

DispatchEventsService& DispatchEventsService::Instance() {
  static DispatchEventsService instance;
  return instance;
}

void DispatchEventsService::RegisterHandler(
    EventType type, std::shared_ptr<IEventHandler> handler) {
  handler_map_[type] = std::move(handler);
}

bool DispatchEventsService::Push(const std::shared_ptr<IEvent>& event) {
  auto event_type = event->type();
  if (handler_map_.find(event_type) == handler_map_.end()) {
    return false;
  }
  return tp_.Drop(std::make_shared<EventHandleTask>(
      event, handler_map_[event_type]));
}

}  // namespace shbk2