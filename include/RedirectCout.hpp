#pragma once

#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>

namespace utils {
// RedirectCout is a utility class that redirects std::cout to a stringstream.
class RedirectCout {
 public:
  RedirectCout() : oldBuffer_(std::cout.rdbuf()) {
    std::cout.rdbuf(buffer_.rdbuf());
  }
  ~RedirectCout() { std::cout.rdbuf(oldBuffer_); }
  RedirectCout(const RedirectCout&) = delete;  // Disable copy constructor
  RedirectCout& operator=(const RedirectCout&) =
      delete;                                        // Disable copy assignment
  RedirectCout(RedirectCout&&) = delete;             // Disable move constructor
  RedirectCout& operator=(RedirectCout&&) = delete;  // Disable move assignment

  std::string str() const { return buffer_.str(); }
  void clear() { buffer_.str(""); }

 private:
  std::streambuf* oldBuffer_;
  std::stringstream buffer_;
};
}  // namespace utils
