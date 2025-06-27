#define CATCH_CONFIG_MAIN

#include <array>
#include <catch2/catch_all.hpp>

#include "SmallStorage.hpp"

using utils::SmallStorage;

TEST_CASE("SmallStorage stack allocation") {
  auto storage = SmallStorage<int, 64>{};
  storage.emplace(42);

  REQUIRE(*storage == 42);
  REQUIRE(storage.isStackAllocated());
  REQUIRE_FALSE(storage.isHeapAllocated());
  REQUIRE_FALSE(storage.empty());
}

TEST_CASE("SmallStorage heap allocation") {
  auto storage = SmallStorage<std::array<int, 8>, 8>{};
  storage.emplace(std::array<int, 8>{
      0, 1, 2, 3, 4, 5, 6, 7});  // Large enough to force heap allocation

  REQUIRE(*storage == std::array<int, 8>{0, 1, 2, 3, 4, 5, 6, 7});
  REQUIRE_FALSE(storage.isStackAllocated());
  REQUIRE(storage.isHeapAllocated());
  REQUIRE_FALSE(storage.empty());
}

TEST_CASE("SmallStorage empty state") {
  auto storage = SmallStorage<int, 64>{};

  REQUIRE(storage.empty());
  REQUIRE_FALSE(storage.isStackAllocated());
  REQUIRE_FALSE(storage.isHeapAllocated());
  REQUIRE(!storage.get());

  // Dereferencing an empty SmallStorage should throw
  REQUIRE_THROWS_AS(*storage, std::runtime_error);
}
TEST_CASE("SmallStorage repeated emplace") {
  auto storage = SmallStorage<int, 64>{};
  storage.emplace(42);
  REQUIRE(*storage == 42);

  storage.emplace(100);
  REQUIRE(*storage == 100);  // Should update the value
  REQUIRE(storage.isStackAllocated());
}

TEST_CASE("SmallStorage move semantics") {
  auto storage = SmallStorage<int, 64>{};
  storage.emplace(42);

  auto movedStorage = std::move(storage);
  REQUIRE(*movedStorage == 42);
  REQUIRE(movedStorage.isStackAllocated());
  REQUIRE(storage.empty());

  movedStorage.emplace(100);
  storage = std::move(movedStorage);
  REQUIRE(*storage == 100);
  REQUIRE(storage.isStackAllocated());
  REQUIRE(movedStorage.empty());

  auto emptyStorage = SmallStorage<int, 64>{};
  storage = std::move(emptyStorage);
  REQUIRE(storage.empty());
  auto anotherEmptyStorage = std::move(emptyStorage);
  REQUIRE(anotherEmptyStorage.empty());
}

TEST_CASE("SmallStorage reset") {
  auto storage = SmallStorage<int, 64>{};
  storage.emplace(42);

  REQUIRE(*storage == 42);
  REQUIRE(!storage.empty());

  storage.reset();
  REQUIRE_THROWS(*storage);
  REQUIRE(storage.empty());
}

TEST_CASE("SmallStorage with derived types") {
  class Base {
   public:
    virtual int getValue() const = 0;
    virtual ~Base() = default;
  };

  class Derived : public Base {
   public:
    explicit Derived(int value) : value_(value) {}
    int getValue() const override { return value_; }

   private:
    int value_;
  };

  class BigDerived : public Derived {
   public:
    explicit BigDerived(int value) : Derived(value) {}

   private:
    int bigData_[1000];  // Large enough to force heap allocation
  };

  auto storage = SmallStorage<Base, 64>{};
  storage.emplace<Derived>(42);

  REQUIRE(storage->getValue() == 42);
  REQUIRE(storage.isStackAllocated());

  storage.emplace<BigDerived>(100);
}
