cmake_minimum_required(VERSION 4.4.3)
project(financial_news_bot VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# MSVC uses a different format for flags than GCC and Clang
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

# Get our dependencies
find_package(cpr CONFIG REQUIRED) # libcpr for handling web requests
find_package(SQLite3 REQUIRED) # sqlite3 so we can save the news articles we Get
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Catch2 3 REQUIRED) # Catch2 unit testing framework

# Library source files (except main.cpp which belongs to the executable)
set(LIB_SOURCE_FILES
    source/Configuration.cpp
    source/NewsRequest.cpp
    source/NewsResponse.cpp
    source/SqliteHandler.cpp
    include/Configuration.h
    include/NewsRequest.h
    include/NewsResponse.h
    include/SqliteHandler.h
)

set(EXEC_SOURCE_FILES
    source/main.cpp
)

# Create library from non-main source files
add_library(financial_news_bot_lib ${LIB_SOURCE_FILES})
target_link_libraries(financial_news_bot_lib PRIVATE cpr::cpr SQLite3::SQLite3)
target_include_directories(financial_news_bot_lib PRIVATE "include")
#target_include_directories(financial_news_bot_lib PRIVATE "${cpr_SOURCE_DIR}/include" "${cpr_BINARY_DIR}/cpr_generated_includes" "${curl_SOURCE_DIR}/include" "${nlohmann_json_SOURCE_DIR}/include")

# Create main executable from main.cpp
add_executable(financial_news_bot ${EXEC_SOURCE_FILES})
target_include_directories(financial_news_bot PRIVATE "include")
target_link_libraries(financial_news_bot PRIVATE financial_news_bot_lib)

# Optional: Add test target if test sources exist
file(GLOB TEST_SOURCES "tests/*.cpp")
if(TEST_SOURCES)
    add_executable(financial_news_bot_test ${TEST_SOURCES})
    target_link_libraries(financial_news_bot_test PRIVATE Catch2::Catch2WithMain financial_news_bot_lib)
    target_include_directories(financial_news_bot_test PRIVATE
        "include"
        "${catch2_SOURCE_DIR}/include"
        "${cpr_SOURCE_DIR}/include"
        "${cpr_BINARY_DIR}/cpr_generated_includes"
        "${curl_SOURCE_DIR}/include"
        "${nlohmann_json_SOURCE_DIR}/include"
    )
    add_test(NAME financial_news_bot_test COMMAND financial_news_bot_test)
endif()
