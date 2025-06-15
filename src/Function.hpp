#pragma once
#include <functional>
#include <string_view>
#include <type_traits>

#include "Lifetime.hpp"
#include "SmallStorage.hpp"

namespace utils {

template <typename Func, typename Return, typename... Args>
concept InvocableWith = std::is_invocable_r_v<Return, Func, Args...>;

template <typename...>
class Function;

template <typename Return, typename... Args>
class Function<Return(Args...)> final
    : public Lifetime<Function<Return(Args...)>> {
 public:
  static constexpr std::string_view getName() { return "Function"; }

  Function() = default;

  template <InvocableWith<Return, Args...> Func>
  explicit Function(Func&& func)
    requires(std::is_rvalue_reference_v<Func &&>)
  {
    callable_.template emplace<CallableDerived<Func>>(std::move(func));
  }

  Return operator()(Args... args) const {
    return callable_->invoke(std::forward<Args>(args)...);
  }

 private:
  class CallableBase : public Lifetime<CallableBase> {
   public:
    static constexpr std::string_view getName() { return "CallableBase"; }
    virtual Return invoke(Args... args) = 0;
  };
  template <InvocableWith<Return, Args...> Func>
  class CallableDerived final : public CallableBase,
                                public Lifetime<CallableDerived<Func>> {
   public:
    static constexpr std::string_view getName() { return "CallableDerived"; }
    CallableDerived(Func&& func) : func_(std::move(func)) {}
    Return invoke(Args... args) override {
      return std::invoke(func_, std::forward<Args>(args)...);
    }

   private:
    Func func_;
  };

  static constexpr int smallStorageSize = 64;
  SmallStorage<CallableBase, smallStorageSize> callable_{};
};
}  // namespace utils