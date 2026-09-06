# AGENTS.md - C++ Project Guidelines

## Build & Test Commands
- Configure: `cmake -B build -DCMAKE_BUILD_TYPE=Release`
- Build: `cmake --build build -j4`
- Test: `cd build && ctest --output-on-failure`

## Coding Standards & Style
- Standard: Use Modern C++ (C++20 preferred, C++17 minimum).
- Style Guide: Follow Google C++ Style Guide or LLVM conventions.
- Memory Safety: Favor smart pointers (`std::unique_ptr`, `std::shared_ptr`). Avoid raw `new`/`delete`.
- Const-correctness: Use `const` by default for variables, parameters, and member functions.

## Quality & Diagnostics
- Run static analysis (`clang-tidy`) before finalizing changes.
- Ensure zero warnings with `-Wall -Wextra -Werror -Wpedantic`.
