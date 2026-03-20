cmake_minimum_required(VERSION 3.16)

# Zentraler Einstiegspunkt fuer Third-Party-Abhaengigkeiten.
add_library(inih_lib
	inih/ini.c
	inih/cpp/INIReader.cpp
	inih/cpp/INIReader.h
)
target_include_directories(inih_lib PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/inih
	${CMAKE_CURRENT_SOURCE_DIR}/inih/cpp
)

set(valijson_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(valijson_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(valijson EXCLUDE_FROM_ALL)

# Create wrapper library for ValiJSON with proper configuration
add_library(valijson_wrapper INTERFACE)
target_link_libraries(valijson_wrapper INTERFACE ValiJSON::valijson)
target_include_directories(valijson_wrapper INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/valijson/include
)

# dotenv-cpp (header-only library)
set(BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(dotenv-cpp EXCLUDE_FROM_ALL)

# Create wrapper library for dotenv-cpp with proper configuration
add_library(dotenv_wrapper INTERFACE)
target_link_libraries(dotenv_wrapper INTERFACE dotenv)
target_include_directories(dotenv_wrapper INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/dotenv-cpp/include
)
