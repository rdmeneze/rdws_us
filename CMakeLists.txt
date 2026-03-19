cmake_minimum_required(VERSION 3.16)

project(rdws_us VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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
add_subdirectory(src/loader)
