# mini asyncio

The absolute bare minimum **toy** asyncio, in **C++** and **Python**!


There is not really any _IO_ in this, simply `mini_asyncio.sleep()` / `mini_asyncio::sleep()`.


## C++ Example:

```cpp
#include <iostream>
#include "mini_asycio.hpp"

auto hello_1() {
  mini_asyncio::Coro sleep;
  return make_generator(({
    while (true) {
      sleep = mini_asyncio::sleep(0.1);
      yield_from(sleep);
      std::cout << "Aww! 1 Wakeup\n";
    }
  }));
}

auto hello_2() {
  mini_asyncio::Coro sleep;
  return make_generator(({
    while (true) {
      sleep = mini_asyncio::sleep(2);
      yield_from(sleep);
      std::cout << "Yay! 2 Wakeup\n";
    }
  }));
}

auto abc() {
  mini_asyncio::Coro sleep;
  return make_generator(({
    while (true) {
      std::cout << "A!\n";
      sleep = mini_asyncio::sleep(1);
      yield_from(sleep);
      std::cout << "B!\n";
      sleep = mini_asyncio::sleep(1);
      yield_from(sleep);
      std::cout << "C!\n";
      sleep = mini_asyncio::sleep(1);
      yield_from(sleep);
    }
  }));
}

int main() {
  auto loop = mini_asyncio::get_event_loop();
  auto task_1 = mini_asyncio::ensure_future(hello_1());
  auto task_2 = mini_asyncio::ensure_future(hello_2());
  auto task_3 = mini_asyncio::ensure_future(abc());
  loop->run_forever();
}
```
