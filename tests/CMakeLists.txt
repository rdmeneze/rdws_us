add_executable(greeting_test greeting_test.cpp)
target_link_libraries(greeting_test PRIVATE rdws_core)

if(TARGET GTest::gtest_main)
  target_link_libraries(greeting_test PRIVATE GTest::gtest_main)
elseif(TARGET gtest_main)
  target_link_libraries(greeting_test PRIVATE gtest_main)
else()
  message(FATAL_ERROR "GoogleTest target not found. Check FetchContent/find_package setup.")
endif()

include(GoogleTest)
gtest_discover_tests(greeting_test)
