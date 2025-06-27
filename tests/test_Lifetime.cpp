#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <string_view>

#include "Lifetime.hpp"
#include "RedirectCout.hpp"

using utils::Lifetime;
using utils::RedirectCout;

struct MyStruct : public Lifetime<MyStruct> {
  static constexpr std::string_view getName() { return "MyStruct"; }
};

TEST_CASE("Lifetime basic usage") {
  auto redirect = RedirectCout{};
  {
    MyStruct myStruct{};
    CHECK(redirect.str() == "MyStruct constructed.\n");
    redirect.clear();
  }
  CHECK(redirect.str() == "MyStruct destroyed.\n");
}

TEST_CASE("Lifetime copy and move semantics") {
  auto redirect = RedirectCout{};
  {
    auto myStruct1 = MyStruct{};
    redirect.clear();

    auto myStruct2 = myStruct1;  // Copy
    CHECK(redirect.str() == "MyStruct copy-constructed.\n");
    redirect.clear();

    auto myStruct3 = std::move(myStruct2);  // Move
    CHECK(redirect.str() == "MyStruct move-constructed.\n");
  }
}
TEST_CASE("Lifetime assignment operators") {
  auto redirect = RedirectCout{};
  {
    auto myStruct1 = MyStruct{};
    redirect.clear();

    auto myStruct2 = MyStruct{};
    redirect.clear();
    myStruct2 = myStruct1;  // Copy assignment
    CHECK(redirect.str() == "MyStruct copy-assigned.\n");
    redirect.clear();

    auto myStruct3 = MyStruct{};
    redirect.clear();
    myStruct3 = std::move(myStruct2);  // Move assignment
    CHECK(redirect.str() == "MyStruct move-assigned.\n");
    redirect.clear();
  }
}
