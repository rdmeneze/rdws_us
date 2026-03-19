cmake_minimum_required(VERSION 3.16)

# --- Testes para componentes shared ---
add_executable(shared_validation_tests
    test_schema_validator.cpp
)

target_link_libraries(shared_validation_tests PRIVATE
    rdws_shared_validation
    gtest_main
    gtest 
)

include(GoogleTest)
gtest_discover_tests(shared_validation_tests)