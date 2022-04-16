#pragma once

#include <optional>

struct StopIteration {};

template<typename TResult>
struct GeneratorResult {
  GeneratorResult(StopIteration) {};
  GeneratorResult(TResult result)
    : result{result}{};
  bool stop_iteration() const { return !result.has_value(); }
  TResult& operator*() { return *result; }
private:
  std::optional<TResult> result;
};

template<>
struct GeneratorResult<void> {
  GeneratorResult() = default;
  GeneratorResult(StopIteration)
    : m_stop{true}{};
  bool stop_iteration() const { return m_stop; }
  void operator*() { return; }
private:
  bool m_stop = false;
};

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)

#define yeild(value) \
  TOKENPASTE2(_yield, __LINE__): if (_rentry != &&TOKENPASTE2(_yield, __LINE__)) { _rentry = &&TOKENPASTE2(_yield, __LINE__); return GeneratorResult(value); } \
  else { _rentry = nullptr; }

#define yield_from(gen)             \
  while (true) {                    \
    auto result = (gen)();          \
    if (result.stop_iteration()) {  \
      break;                        \
    }                               \
    yeild(*result);                 \
  }

#define greturn \
  {GeneratorResult(StopIteration{});}

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
