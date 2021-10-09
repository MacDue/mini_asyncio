#pragma once

#include <functional>

#include "future.hpp"

namespace mini_asyncio {
using Coro = std::function<Future*()>;

class Task: public Future {

  Coro _coro;

  void _step();

  void _wakeup(EventLoop& _) {
    this->_step();
  }

  public:
    Task(Coro coro, EventLoop& loop);
};

}
