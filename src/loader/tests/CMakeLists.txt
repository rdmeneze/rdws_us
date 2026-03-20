add_executable(loader_test loader_test.cpp
        test_service_schema.cpp
        test_services_loader.cpp
        ../Services/Services.cpp
        ../Services/Services.h)
target_link_libraries(loader_test PRIVATE loader_core)

if(TARGET GTest::gtest_main)
    target_link_libraries(loader_test PRIVATE GTest::gtest_main)
elseif(TARGET gtest_main)
    target_link_libraries(loader_test PRIVATE gtest_main)
else()
    message(FATAL_ERROR "GoogleTest target not found. Check FetchContent/find_package setup.")
endif()

include(GoogleTest)
gtest_discover_tests(loader_test)
