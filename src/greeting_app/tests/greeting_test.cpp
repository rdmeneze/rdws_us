#include <gtest/gtest.h>

#include "greeting.hpp"

TEST(GreetingTest, ReturnsWorldWhenNameIsEmpty) {
  EXPECT_EQ(rdws_us::make_greeting(""), "Hello, world!");
}

TEST(GreetingTest, ReturnsNamedGreeting) {
  EXPECT_EQ(rdws_us::make_greeting("rdias"), "Hello, rdias!");
}
