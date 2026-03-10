#include <iostream>

#include "loader.hpp"

int main() {
  std::cout << rdws_us::loader::build_source_uri("config") << '\n';
  return 0;
}

