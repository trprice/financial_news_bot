cmake_minimum_required(VERSION 4.4.3)
project(financial_news_bot VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# MSVC uses a different format for flags than GCC and Clang
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

include(FetchContent)

# Download and build CPR
FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG        1.14.2 # Use the latest stable version tag
)
FetchContent_MakeAvailable(cpr)
FetchContent_MakeAvailable(curl)

# Download and make Sqlite3 available
FetchContent_Declare(
    sqlite3
    GIT_REPOSITORY https://github.com/sjinks/sqlite3-cmake
    GIT_TAG        v3.49.1
)

FetchContent_MakeAvailable(sqlite3)

# Download and make Nlohmann/json available
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG        v3.12.0
)

FetchContent_MakeAvailable(nlohmann_json)

# List source files (the directory structure we placed earlier)
set(SOURCE_FILES
    source/main.cpp
    source/Configuration.cpp
    source/Configuration.h
    source/NewsRequest.cpp
    source/NewsRequest.h
    source/NewsResponse.cpp
    source/NewsResponse.h
    source/SqliteHandler.cpp
    source/SqliteHandler.h)

add_executable(financial_news_bot ${SOURCE_FILES})
target_link_libraries(financial_news_bot PRIVATE cpr SQLite3)
target_include_directories(financial_news_bot PRIVATE "${cpr_SOURCE_DIR}/include" "${cpr_BINARY_DIR}/cpr_generated_includes" "${curl_SOURCE_DIR}/include" "${nlohmann_json_SOURCE_DIR}/include")
