#include "task.hpp"
#include "event_loop.hpp"
#include <iostream>

namespace mini_asyncio {
  Task::Task(Coro coro, EventLoop& loop): Future{loop}, _coro{coro}
  {
    this->_loop.call_soon(&Task::_step, this);
  }

  void Task::_step() {
    try {
      auto result = this->_coro();
      if (result == nullptr) {
        this->_loop.call_soon(&Task::_step, this);
      } else {
        if (result->_blocking) {
          result->add_done_callback(
            std::bind(&Task::_wakeup, this, std::placeholders::_1));
        }
      }
    } catch (StopIteration) {
      this->set_done();
    }
  }
}
