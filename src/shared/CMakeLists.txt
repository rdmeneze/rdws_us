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
    ValiJSON::valijson
)

# Required for jsoncpp (used by valijson adapters)
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)

if(JSONCPP_FOUND)
    target_link_libraries(rdws_shared_validation PUBLIC ${JSONCPP_LIBRARIES})
    target_include_directories(rdws_shared_validation PUBLIC ${JSONCPP_INCLUDE_DIRS})
    target_compile_options(rdws_shared_validation PUBLIC ${JSONCPP_CFLAGS_OTHER})
else()
    message(WARNING "jsoncpp not found via pkg-config, validation may not work correctly")
endif()

# --- Testes para shared components ---
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()