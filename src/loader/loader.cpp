#include "loader.hpp"

namespace rdws_us::loader {

std::string build_source_uri(const std::string& source) {
  if (source.empty()) {
    return "loader://default";
  }

  return "loader://" + source;
}

}  // namespace rdws_us::loader

