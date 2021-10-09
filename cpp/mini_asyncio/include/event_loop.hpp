#pragma once

#include <queue>
#include <deque>
#include <chrono>
#include <memory>
#include <functional>

#include "task.hpp"
#include "future.hpp"
#include "callbacks.hpp"
#include "generator.hpp"

namespace mini_asyncio {

using Timepoint = std::chrono::time_point<std::chrono::system_clock>;

struct Timer {
  Timepoint when;
  Callback callback;

  Timer(Timepoint when, Callback callback)
    : when{when}, callback{callback} {}

  inline bool operator >(Timer const &  rhs) const {
    return this->when > rhs.when;
  }
};

class EventLoop {
  std::deque<Callback> _ready_cbs;
  std::priority_queue<Timer, std::vector<Timer>, std::greater<Timer>> _timer_cbs;
  bool _stopping = false;

  void _run_once();

public:
  EventLoop() = default;

  inline void stop() {
    this->_stopping = true;
  }

  template<typename TCallback, typename ...TArgs>
  void call_soon(TCallback cb, TArgs... args) {
    this->_ready_cbs.push_back(std::bind(cb, args...));
  }

  template<typename TCallback, typename ...TArgs>
  void call_later(float delay, TCallback cb, TArgs... args) {
    using namespace std::chrono;
    auto now = system_clock::now();
    this->_timer_cbs.push(Timer(
      now + milliseconds(static_cast<uint64_t>(delay * 1000)),
      std::bind(cb, args...)
    ));
  }

  inline std::shared_ptr<Task> create_task(Coro coro) {
    return std::make_shared<Task>(coro, *this);
  }

  inline std::shared_ptr<Future> create_future() {
    return std::make_shared<Future>(*this);
  }

  void run_forever();
};

EventLoop* get_event_loop();

std::shared_ptr<Task> ensure_future(Coro coro, EventLoop* loop=nullptr);

Coro sleep(float delay, EventLoop* loop=nullptr);

}
