# rdws_us

Projeto C++ modular com multiplos subprojetos em `src/`, cada um com:

- biblioteca interna (`<nome>_core`)
- executavel (`<nome>_app`)
- testes unitarios (`<nome>_test`)

## Estrutura atual

- `CMakeLists.txt`: configuracao raiz e dependencia de GoogleTest.
- `src/greeting_app/`: subprojeto de exemplo.
- `src/loader/`: subprojeto de exemplo.
- `tools/new_subproject.sh`: gerador de novo subprojeto.

## Build e testes

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Criar um novo subprojeto em 1 minuto

```bash
./tools/new_subproject.sh auth
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Isso cria:

- `src/auth/CMakeLists.txt`
- `src/auth/auth.hpp`
- `src/auth/auth.cpp`
- `src/auth/main.cpp`
- `src/auth/tests/CMakeLists.txt`
- `src/auth/tests/auth_test.cpp`

E adiciona automaticamente `add_subdirectory(src/auth)` ao `CMakeLists.txt` raiz.
