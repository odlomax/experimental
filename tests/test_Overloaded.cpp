#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <string>
#include <variant>

#include "Overloaded.hpp"

using utils::Overloaded;

TEST_CASE("Overloaded basic usage") {
  auto overloaded =
      Overloaded{[](int x) { return x + 1; }, [](double x) { return x + 2.0; },
                 [](const std::string& s) { return s + " world"; }};

  REQUIRE(overloaded(1) == 2);
  REQUIRE(overloaded(3.14) == Catch::Approx(5.14).epsilon(1e-12));
  REQUIRE(overloaded(std::string("Hello")) == "Hello world");
}

TEST_CASE("Overloaded with std::variant") {
  using VariantType = std::variant<int, double, std::string>;
  auto overloaded =
      Overloaded{[](int x) { return std::to_string(x + 1); },
                 [](double x) { return std::to_string(x + 2.0); },
                 [](const std::string& s) { return s + " world"; }};

  VariantType v1 = 42;
  VariantType v2 = 3.14;
  VariantType v3 = std::string("Hello");

  REQUIRE(std::visit(overloaded, v1) == "43");
  REQUIRE(std::visit(overloaded, v2) == "5.140000");
  REQUIRE(std::visit(overloaded, v3) == "Hello world");
}
