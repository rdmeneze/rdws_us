cmake_minimum_required(VERSION 3.16)

project(rdws_us VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS ON)

# Configure debug settings
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
    message(STATUS "Debug mode enabled with symbols")
endif()

include(CTest)
if(BUILD_TESTING)
  include(FetchContent)

  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )
  FetchContent_MakeAvailable(googletest)
endif()


# --- Third-party ---
add_subdirectory(src/third_party)

# --- Shared Components ---
add_subdirectory(src/shared)

# --- Subprojetos ---
add_subdirectory(src/greeting_app)
add_subdirectory(src/service_broker)

# --- Services ---
add_subdirectory(src/services)

# --- Copy global configuration files ---
add_custom_target(copy_env_file ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/.env.dev
        ${CMAKE_BINARY_DIR}/.env
    COMMENT "Copying .env.dev to build directory as .env"
)
