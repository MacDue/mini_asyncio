#include "future.hpp"
#include "event_loop.hpp"

namespace mini_asyncio {

void Future::_schedule_callbacks() {
  for (auto& cb: this->_done_cbs) {
    this->_loop.call_soon(cb, this->_loop);
  }
}

void Future::add_done_callback(Future::DoneCallback callback) {
  if (!this->_done) {
    this->_done_cbs.push_back(callback);
  } else {
    this->_loop.call_soon(callback, this->_loop);
  }
}

}
