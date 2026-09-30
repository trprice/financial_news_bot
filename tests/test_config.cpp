#include "catch2/catch_test_macros.hpp"
#include "Configuration.h"
#include <iostream>

TEST_CASE("Configuration can be initialized and has getters") {
    Configuration cfg("tests/config.json");
    
    // Test that getApiToken() works and returns default value
    std::string token = cfg.getApiToken();
    
    // Test that getTickers() returns empty vector
    const std::vector<std::string>& tickers = cfg.getTickers();
    
    REQUIRE(token.length() > 0); // token should be non-empty
    REQUIRE(tickers.size() == 0); // initially empty
    
    // Test getApiToken() returns token
    REQUIRE(token.size() >= 8); // reasonable min length
    
    // Test getTickers() returns empty
    REQUIRE(tickers.size() == 0);
}
