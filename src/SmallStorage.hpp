#pragma once

#include <concepts>
#include <memory>
#include <type_traits>
#include <variant>

#include "Lifetime.hpp"
#include "Overloaded.hpp"

namespace utils {

template <typename Type, int Size>
class SmallStorage final {
 public:
  SmallStorage() = default;

  template <std::derived_from<Type> EmplacedType, typename... Args>
  void emplace(Args&&... args) {
    if (sizeof(EmplacedType) > Size) {
      storage_.template emplace<HeapStorage>(
          new EmplacedType(std::forward<Args>(args)...));
    } else {
      storage_.template emplace<StackStorage>(std::in_place_type<EmplacedType>,
                                              std::forward<Args>(args)...);
    }
  }

  Type* operator->() const {
    return std::visit(
        utils::Overloaded{
            [](const std::monostate&) -> Type* { return nullptr; },
            [](const auto& storage) -> Type* { return storage.operator->(); },
        },
        storage_);
  }

 private:
  class StackStorage : public Lifetime<StackStorage> {
   public:
    static constexpr std::string_view getName() { return "StackStorage"; }
    template <typename EmplacedType, typename... Args>
    StackStorage(std::in_place_type_t<EmplacedType>, Args&&... args) {
      std::construct_at(reinterpret_cast<EmplacedType*>(&storage_),
                        std::forward<Args>(args)...);
    }
    ~StackStorage() { std::destroy_at(reinterpret_cast<Type*>(&storage_)); }
    StackStorage(const StackStorage&) = default;
    StackStorage(StackStorage&&) = default;
    StackStorage& operator=(const StackStorage&) = default;
    StackStorage& operator=(StackStorage&&) = default;
    Type* operator->() const { return reinterpret_cast<Type*>(&storage_); }

   private:
    // Make this mutable so it behaves like heap storage.
    mutable std::aligned_storage_t<Size> storage_{};
  };
  using HeapStorage = std::unique_ptr<Type>;
  mutable std::variant<std::monostate, StackStorage, HeapStorage> storage_{};
};
}  // namespace utils
