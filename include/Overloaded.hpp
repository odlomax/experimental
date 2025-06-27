#pragma once

namespace utils {

template <typename... Funcs>
struct Overloaded : Funcs... {
  using Funcs::operator()...;
};
}  // namespace utils
