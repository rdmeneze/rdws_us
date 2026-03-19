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
