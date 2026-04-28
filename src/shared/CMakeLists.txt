cmake_minimum_required(VERSION 3.16)

# --- Shared Validation Library ---
add_library(rdws_shared_validation
    validator/schema_validator.cpp
    validator/schema_validator.h
)

target_include_directories(rdws_shared_validation PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/validator
)

target_link_libraries(rdws_shared_validation PUBLIC
    valijson_wrapper
)

# jsoncpp is provided via valijson_wrapper (bundled)


add_library(rdws_shared_config 
    config/config.cpp
    config/config.h
)

target_include_directories(rdws_shared_config PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(rdws_shared_config PUBLIC
    dotenv_wrapper
)

# --- Testes para shared components ---
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()