cmake_minimum_required(VERSION 4.4.3)
project(financial_news_bot VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# MSVC uses a different format for flags than GCC and Clang
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

# Locate CPR and SQLite3
find_package(CPR REQUIRED)
target_link_librariesfind_package(SQLite3 REQUIRED)

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
target_link_libraries(financial_news_bot PRIVATE CPR SQLite3)
