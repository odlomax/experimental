#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <string_view>

#include "Function.hpp"
#include "Lifetime.hpp"
#include "RedirectCout.hpp"

using utils::Function;
int add2(int x) { return x + 2; }
struct MyStruct : public utils::Lifetime<MyStruct> {
  static constexpr std::string_view getName() { return "MyStruct"; }
  int operator()(int x) const { return x + 3; }
  double data = 3.;
};

TEST_CASE("utils::Function basic usage", "[Function]") {
  SECTION("Default constructed Function throws on call") {
    auto f = Function<int(int)>{};

    REQUIRE_THROWS_AS(f(1), std::bad_function_call);
    REQUIRE(sizeof(f) == 64);
  }

  SECTION("Function wraps lambda and calls correctly") {
    auto f = Function<int(int)>([](int x) { return x + 1; });
    REQUIRE(f(2) == 3);
    REQUIRE(f(10) == 11);
  }

  SECTION("Function wraps function pointer") {
    auto f = Function<int(int)>(add2);
    REQUIRE(f(5) == 7);
  }

  SECTION("Function wraps mutable lambda") {
    auto f = Function<int(int)>([state = 0](int x) mutable {
      state += x;
      return state;
    });
    REQUIRE(f(1) == 1);
    REQUIRE(f(2) == 3);
  }

  SECTION("Function with multiple arguments") {
    auto f = Function<std::string(const std::string&, int)>(
        [](const std::string& s, int n) { return s + std::to_string(n); });
    REQUIRE(f("test", 42) == "test42");
  }

  SECTION("Struct member function") {
    auto myStruct = MyStruct{};
    auto f = Function<int(MyStruct&, int)>(&MyStruct::operator());
    REQUIRE(f(myStruct, 4) == 7);  // 4 + 3 from MyStruct
  }

  SECTION("Struct member accessor") {
    auto myStruct = MyStruct{};
    auto f = Function<double(MyStruct&)>(&MyStruct::data);
    REQUIRE(f(myStruct) == 3.0);  // Accessing data member
  }

  SECTION("Test move semantics") {
    auto f = Function<int(int)>([](int x) { return x + 1; });
    auto g = std::move(f);
    auto h = Function<int(int)>{};
    h = std::move(g);
    REQUIRE_THROWS_AS(f(1), std::bad_function_call);  // f should be invalid
    REQUIRE_THROWS_AS(g(1), std::bad_function_call);  // g should be invalid
    REQUIRE(h(3) == 4);  // Ensure the moved function works
  }

  SECTION("Function with moved struct") {
    auto redirect = utils::RedirectCout{};
    {
      Function<int(int)> f(MyStruct{});
      REQUIRE(f(4) == 7);  // 4 + 3 from MyStruct
    }
    REQUIRE(redirect.str() ==
            "MyStruct constructed.\n"
            "MyStruct move-constructed.\n"
            "MyStruct destroyed.\n"
            "MyStruct destroyed.\n");
  }

  SECTION("Function with copied struct") {
    auto redirect = utils::RedirectCout{};
    {
      auto myStruct = MyStruct{};
      Function<int(int)> f(myStruct);
      REQUIRE(f(4) == 7);  // 4 + 3 from MyStruct
    }
    REQUIRE(redirect.str() ==
            "MyStruct constructed.\n"
            "MyStruct copy-constructed.\n"
            "MyStruct destroyed.\n"
            "MyStruct destroyed.\n");
  }
}
