#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <string_view>

#include "src/Function.hpp"
#include "src/Lifetime.hpp"
#include "src/RedirectCout.hpp"

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
    Function<int(int)> f([](int x) { return x + 1; });
    REQUIRE(f(2) == 3);
    REQUIRE(f(10) == 11);
  }

  SECTION("Function wraps function pointer") {
    Function<int(int)> f(add2);
    REQUIRE(f(5) == 7);
  }

  SECTION("Function wraps mutable lambda") {
    Function<int(int)> f([state = 0](int x) mutable {
      state += x;
      return state;
    });
    REQUIRE(f(1) == 1);
    REQUIRE(f(2) == 3);
  }

  SECTION("Function with multiple arguments") {
    Function<std::string(const std::string&, int)> f(
        [](const std::string& s, int n) { return s + std::to_string(n); });
    REQUIRE(f("test", 42) == "test42");
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
