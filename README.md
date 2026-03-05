# rdws_us

Erster Entwurf eines Orchestrators für Microservices, geschrieben in modernem C++. Ziel ist es, einige Dienste für das 
Abfragen, Lesen, Aktualisieren und Löschen von Datensätzen in einigen Tabellen einer Datenbank  auf einem lokalen Server 
mit Fedora Server bereitzustellen.

## Struktur

- `CMakeLists.txt`: Build-Root-Konfiguration und Integration von GoogleTest (FetchContent).
- `include/rdws_us/greeting.hpp`: interface publica.
- `src/greeting.cpp`: implementacao da biblioteca.
- `src/main.cpp`: programa principal.
- `tests/greeting_test.cpp`: testes com GoogleTest.

## Schneller Aufbau

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## App ausführen

```bash
./build/rdws_app
```
# rdws_us
