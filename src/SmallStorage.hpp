#pragma once

#include <algorithm>
#include <array>
#include <concepts>
#include <memory>
#include <variant>

#include "Overloaded.hpp"

namespace utils {

template <typename EmplacedType, typename Type>
concept EmplacableType =
    std::derived_from<EmplacedType, Type> || std::same_as<EmplacedType, Type>;

template <typename Type, int Size>
class SmallStorage {
 public:
  SmallStorage() = default;

  template <EmplacableType<Type> EmplacedType = Type, typename... Args>
  SmallStorage& emplace(Args&&... args) {
    if (sizeof(EmplacedType) > StackStorageSize) {
      storage_.template emplace<HeapStorage>(
          new EmplacedType(std::forward<Args>(args)...));
    } else {
      storage_.template emplace<StackStorage>(std::in_place_type<EmplacedType>,
                                              std::forward<Args>(args)...);
    }
    return *this;
  }

  Type* get() const {
    return std::visit(
        utils::Overloaded{
            [](std::monostate) -> Type* { return nullptr; },
            [](const auto& storage) -> Type* { return storage.get(); },
        },
        storage_);
  }

  Type* operator->() const {
    if (empty()) {
      throw std::runtime_error("Dereferencing empty SmallStorage");
    }
    return get();
  }

  Type& operator*() const { return *operator->(); }

  bool empty() const {
    return std::visit(
        utils::Overloaded{[](std::monostate) -> bool { return true; },
                          [](const auto& storage) -> bool {
                            return !storage.operator bool();
                          }},
        storage_);
  }

  void reset() {
    std::visit(utils::Overloaded{[](std::monostate) {},
                                 [](auto& storage) { storage.reset(); }},
               storage_);
  }

  bool isStackAllocated() const {
    return std::holds_alternative<StackStorage>(storage_) && !empty();
  }
  bool isHeapAllocated() const {
    return std::holds_alternative<HeapStorage>(storage_) && !empty();
  }

 private:
  using HeapStorage = std::unique_ptr<Type>;
  // Set stack size to whichever is larger: the size of the heap storage or the
  // size of the Size parameter minus the size of variant index.
  static constexpr auto StackStorageSize =
      std::max(sizeof(HeapStorage), Size - sizeof(std::size_t));

  class StackStorage {
   public:
    template <EmplacableType<Type> EmplacedType, typename... Args>
    StackStorage(std::in_place_type_t<EmplacedType>, Args&&... args) {
      new (storage_.data()) EmplacedType(std::forward<Args>(args)...);
    }
    ~StackStorage() { reset(); }
    StackStorage(const StackStorage&) = delete;
    StackStorage(StackStorage&& other) noexcept {
      if (!other) {
        return;  // If other is empty, do nothing
      }
      new (storage_.data()) Type(std::move(*other.get()));
      other.reset();
    };
    StackStorage& operator=(const StackStorage&) = delete;
    StackStorage& operator=(StackStorage&& other) noexcept {
      if (!other || this == &other) {
        return *this;  // If other is empty or self-assignment, do nothing
      }
      reset();
      new (storage_.data()) Type(std::move(*other.get()));
      other.reset();
      return *this;
    };
    Type* get() const { return reinterpret_cast<Type*>(storage_.data()); }
    operator bool() const {
      // Check for non-zero bytes.
      return std::ranges::any_of(
          storage_, [](std::byte b) -> bool { return b != std::byte{}; });
    }
    void reset() {
      if (operator bool()) {
        get()->~Type();
        storage_.fill(std::byte{});
      }
    }

   private:
    // Make this mutable so it behaves like heap storage.
    mutable std::array<std::byte, StackStorageSize> storage_{};
  };
  std::variant<std::monostate, StackStorage, HeapStorage> storage_{};
};
}  // namespace utils
