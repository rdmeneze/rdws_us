#include <gtest/gtest.h>

#include "loader.hpp"

TEST(LoaderTest, ReturnsDefaultUriWhenSourceIsEmpty) {
  EXPECT_EQ(rdws_us::loader::build_source_uri(""), "loader://default");
}

TEST(LoaderTest, PrefixesNonEmptySourceWithScheme) {
  EXPECT_EQ(rdws_us::loader::build_source_uri("users.csv"), "loader://users.csv");
}

