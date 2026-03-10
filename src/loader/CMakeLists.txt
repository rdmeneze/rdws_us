# --- Biblioteca interna do subprojeto ---
add_library(loader_core
    loader.cpp
)
target_include_directories(loader_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# --- Executavel ---
add_executable(loader_app main.cpp)
target_link_libraries(loader_app PRIVATE loader_core)

# --- Testes unitarios ---
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

