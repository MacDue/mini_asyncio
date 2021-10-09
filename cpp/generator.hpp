#pragma once

#include <exception>

struct StopIteration : public std::exception {
  const char* what() const noexcept {
    return "StopIteration";
  }
};

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)

#define yeild(value) \
  TOKENPASTE2(_yield, __LINE__): if (_rentry != &&TOKENPASTE2(_yield, __LINE__)) { _rentry = &&TOKENPASTE2(_yield, __LINE__); return (value); } \
  else { _rentry = nullptr; }

#define yield_from(gen)           \
  while (true) {                  \
    try {                         \
      yeild((gen)());             \
    } catch (StopIteration) {     \
      break;                      \
    }                             \
  }

#define greturn \
  {throw StopIteration{};}

#define make_generator(body)       \
  ({                               \
      void *_rentry = nullptr;     \
      auto _gen = [=]() mutable {  \
        if (_rentry) {             \
          goto *_rentry;           \
        }                          \
        body;                      \
        greturn;                   \
      };                           \
      _gen;                        \
  })
