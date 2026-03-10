# --- Biblioteca interna do subprojeto ---
add_library(greeting_core
    greeting.cpp
)
target_include_directories(greeting_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# --- Executável ---
add_executable(greeting_app main.cpp)
target_link_libraries(greeting_app PRIVATE greeting_core)

# --- Testes unitários ---
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

