add_executable(service_broker_test 
        test_service_schema.cpp
)
target_link_libraries(service_broker_test PRIVATE service_broker_core)

if(TARGET GTest::gtest_main)
    target_link_libraries(service_broker_test PRIVATE GTest::gtest_main)
elseif(TARGET gtest_main)
    target_link_libraries(service_broker_test PRIVATE gtest_main)
else()
    message(FATAL_ERROR "GoogleTest target not found. Check FetchContent/find_package setup.")
endif()

include(GoogleTest)
gtest_discover_tests(service_broker_test)
