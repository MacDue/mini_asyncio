
#include <iostream>
#include "generator.hpp"


auto gen_count(int num) {
  int i;
  return make_generator(({
    for (i = 0; i < num; i++) {
      yeild(i);
    }
    std::cout << "Loop done!\n";
    for (i = num; i >= 0; i--) {
      yeild(i);
    }
  }));
}

auto gen_scaler(auto gen, int scale) {
  return make_generator(({
    while (true) {
      try {
        yeild(gen() * scale)
      } catch (StopIteration) {
        break;
      }
    }
  }));
}

// int main() {
//   auto gen = gen_count(5);
//   while (true) {
//     try {
//       std::cout << gen() << '\n';
//     } catch (StopIteration) {
//       break;
//     }
//   }
// }


int main() {
  auto doubled = gen_scaler(gen_count(5), 2);
  while (true) {
    try {
      std::cout << doubled() << '\n';
    } catch (StopIteration) {
      break;
    }
  }
}
