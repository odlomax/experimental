#pragma once
#include <iostream>

namespace utils {
template <typename Derived>
class Lifetime {
 public:
  Lifetime() { std::cout << Derived::getName() << " constructed.\n"; }
  Lifetime(const Lifetime&) {
    std::cout << Derived::getName() << " copy-constructed.\n";
  }
  Lifetime(Lifetime&&) {
    std::cout << Derived::getName() << " move-constructed.\n";
  }
  Lifetime& operator=(const Lifetime&) {
    std::cout << Derived::getName() << " copy-assigned.\n";
    return *this;
  }
  Lifetime& operator=(Lifetime&&) noexcept {
    std::cout << Derived::getName() << " move-assigned.\n";
    return *this;
  }
  virtual ~Lifetime() { std::cout << Derived::getName() << " destroyed.\n"; };
};
}  // namespace utils
