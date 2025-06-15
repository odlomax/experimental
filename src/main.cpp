#define PRINT_LIFETIME

#include "Function.hpp"
#include "Lifetime.hpp"
#include <string_view>

struct Derived : public utils::Lifetime<Derived> {
  static constexpr std::string_view getName() { return "Derived"; }
};

auto myFunction(int x) -> int {
  return x * 2;
}

struct MyStruct : public utils::Lifetime<MyStruct> {
  MyStruct(double a = 0.0) : a_(a) {}
  double a_;
  static constexpr std::string_view getName() { return "MyStruct"; }
  int operator()(int x) const { return x * 2; }
};

int main() {

  auto lambda = [a = std::array<int, 128>{}](int x) mutable {
    ++a[0];
    return x * 2;
  };

  const auto s = utils::Function<int(int)>{MyStruct{}};
  const auto t = utils::Function<int(int)>{std::move(lambda)};
  const auto u = utils::Function<int(const MyStruct&, int)>(&MyStruct::operator());
  const auto v = utils::Function<double(const MyStruct&)>(&MyStruct::a_);
  const auto w = utils::Function<int(int)>{&myFunction};

  std::cout << s(3) << "\n";
  std::cout << t(4) << "\n";
  std::cout << u(MyStruct{1.0}, 5) << "\n";
  std::cout << v(MyStruct{2.0}) << "\n";
  std::cout << w(6) << "\n";

  std::cout << "Function size: " << sizeof(utils::Function<int(int)>) << "\n";
}