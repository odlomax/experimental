#pragma once
#include <functional>
#include <type_traits>
#include <utility>

#include "SmallStorage.hpp"

namespace utils {

template <typename Func, typename Return, typename... Args>
concept InvocableWith = std::is_invocable_r_v<Return, Func, Args...>;

template <typename...>
class Function;

template <typename Return, typename... Args>
class Function<Return(Args...)> {
 public:
  Function() = default;

  template <InvocableWith<Return, Args...> Func>
  explicit Function(Func&& func) {
    using FuncType = std::remove_cvref_t<Func>;
    callable_.template emplace<CallableDerived<FuncType>>(
        std::forward<Func>(func));
  }

  Function(Return (*func)(Args...)) {
    callable_.template emplace<CallableDerived<Return (*)(Args...)>>(func);
  }

  Return operator()(Args... args) const {
    if (callable_.empty()) {
      throw std::bad_function_call();
    }
    return callable_->invoke(std::forward<Args>(args)...);
  }

 private:
  class CallableBase {
    public:
    explicit CallableBase() = default;
    explicit CallableBase(const CallableBase&) = default;
    explicit CallableBase(CallableBase&&) = default;
    CallableBase& operator=(const CallableBase&) = default;
    CallableBase& operator=(CallableBase&&) = default;
    virtual ~CallableBase() = default;
    virtual Return invoke(Args... args) = 0;
  };
  template <InvocableWith<Return, Args...> FuncType>
  class CallableDerived final : public CallableBase {
   public:
    template <InvocableWith<Return, Args...> FuncForwardType>
    CallableDerived(FuncForwardType&& func)
        : func_(std::forward<FuncForwardType>(func)) {}

    Return invoke(Args... args) override {
      return std::invoke(func_, std::forward<Args>(args)...);
    }

   private:
    FuncType func_;
  };

  static constexpr int smallStorageSize = 64;
  SmallStorage<CallableBase, smallStorageSize> callable_{};
};
}  // namespace utils