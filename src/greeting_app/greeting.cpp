#include "greeting.hpp"

namespace rdws_us {

std::string make_greeting(const std::string& name) {
  if (name.empty()) {
    return "Hello, world!";
  }

  return "Hello, " + name + "!";
}

}  // namespace rdws_us

