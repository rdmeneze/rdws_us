cmake_minimum_required(VERSION 3.16)

project(rdws_us VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(rdws_core src/greeting.cpp)
target_include_directories(rdws_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(rdws_app src/main.cpp)
target_link_libraries(rdws_app PRIVATE rdws_core)

include(CTest)
if(BUILD_TESTING)
  include(FetchContent)

  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )
  FetchContent_MakeAvailable(googletest)

  add_subdirectory(tests)
endif()
