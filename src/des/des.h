#ifndef SHBK2_DISPATCH_EVENT_SERVICE_H_
#define SHBK2_DISPATCH_EVENT_SERVICE_H_

#include <utility>
#include <vector>
#include <memory>
#include <unordered_map>

#include "events.h"
#include "tinys/tiny_tp.hpp"
#include "event_handlers.h"

namespace shbk2 {

class EventHandleTask : public tiny_tp::ITask {
 public:
  EventHandleTask(std::shared_ptr<IEvent> event,
                  std::shared_ptr<IEventHandler> handler)
      : event_{std::move(event)}, handler_{std::move(handler)} {}
  std::shared_ptr<void> Execute() final { return handler_->Handle(event_); }
 private:
  std::shared_ptr<IEvent> event_;
  std::shared_ptr<IEventHandler> handler_;
};

class DispatchEventsService {
 public:
  static DispatchEventsService& Instance();

  void RegisterHandler(EventType type, std::shared_ptr<IEventHandler> handler);
  bool Push(const std::shared_ptr<IEvent>& event);
  std::vector<std::shared_ptr<void>> Drag() { return tp_.GrabAllResults(); }

 private:
  DispatchEventsService() = default;

  tiny_tp::ThreadPool tp_;
  std::unordered_map<EventType, std::shared_ptr<IEventHandler>> handler_map_;
};

}  // namespace shbk2

#endif  // SHBK2_DISPATCH_EVENT_SERVICE_H_