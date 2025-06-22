#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <iostream>

#include "RedirectCout.hpp"

using utils::RedirectCout;

TEST_CASE("RedirectCout captures std::cout output") {
  auto testStr = "Hello, redirected cout!";

  auto redirect = RedirectCout();
  std::cout << testStr;

  REQUIRE(redirect.str() == testStr);
}

TEST_CASE("RedirectCout clears buffer") {
  auto testStr = "This will be cleared";

  auto redirect = RedirectCout();
  std::cout << testStr;

  REQUIRE(redirect.str() == testStr);

  redirect.clear();
  REQUIRE(redirect.str().empty());
}

TEST_CASE("RedirectCout restores std::cout") {
  auto testStr = "This should not be captured";

  {
    auto redirect = RedirectCout();
    std::cout << testStr;
  }  // RedirectCout goes out of scope and restores std::cout

  std::cout << "Normal service has been resumed\n";
}