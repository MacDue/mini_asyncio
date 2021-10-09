#pragma once

#include <any>
#include <vector>
#include <optional>
#include <functional>
#include <iostream>

#include "generator.hpp"

namespace mini_asyncio {

class EventLoop;

class Future {
public:
  using DoneCallback = std::function<void(EventLoop&)>;
protected:
  EventLoop& _loop;
private:
  bool _done = false;
  std::vector<DoneCallback> _done_cbs;
  void _schedule_callbacks();
public:
  bool _blocking = false;
  Future(EventLoop& loop): _loop{loop} {}

  void add_done_callback(DoneCallback callback);

  void set_done() {
    this->_done = true;
    this->_schedule_callbacks();
  }

  std::function<Future*()> operator() () {
    return make_generator({
      while (!this->_done) {
        this->_blocking = true;
        yeild(this);
      }
    });
  }
};

}
