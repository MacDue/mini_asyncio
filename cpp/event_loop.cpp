#include <thread>
#include <algorithm>
#include "event_loop.hpp"

namespace mini_asyncio {

void EventLoop::_run_once() {
  using namespace std::chrono;

  auto timeout = this->_timer_cbs.empty()
    ? milliseconds(0)
    : duration_cast<milliseconds>(this->_timer_cbs.top().when - system_clock::now());

  if (this->_ready_cbs.empty()) {
    timeout = std::max(milliseconds(0), timeout);
    std::this_thread::sleep_for(timeout);
  }

  while (
    !this->_timer_cbs.empty()
    && this->_timer_cbs.top().when <= system_clock::now()
  ) {
    auto& timer = this->_timer_cbs.top();
    this->_ready_cbs.push_back(timer.callback);
    this->_timer_cbs.pop();
  }

  auto num_todo = this->_ready_cbs.size();
  while (num_todo > 0) {
    auto callback = this->_ready_cbs.front();
    this->_ready_cbs.pop_front();
    callback();
    --num_todo;
  }
}

void EventLoop::run_forever() {
  while (true) {
    this->_run_once();
    if (this->_stopping) {
      break;
    }
  }
  this->_stopping = false;
}

EventLoop* get_event_loop() {
  static EventLoop _current_loop;
  return &_current_loop;
}

std::shared_ptr<Task> ensure_future(Coro coro, EventLoop* loop) {
  if (loop == nullptr) {
    loop = get_event_loop();
  }

  return loop->create_task(coro);
}

Coro sleep(float delay, EventLoop* loop) {
  if (loop == nullptr) {
    loop = get_event_loop();
  }
  auto future = loop->create_future();
  return make_generator(({
    loop->call_later(delay, [&]{
      future->set_done();
    });

    yield_from((*future)());
  }));
}

}
