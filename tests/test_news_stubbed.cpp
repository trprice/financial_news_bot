#include "catch2/catch_test_macros.hpp"
#include "Configuration.h"
#include "NewsRequest.h"
#include "httpServer.hpp"
#include <iostream>
#include <string>

TEST_CASE("NewsRequest stubbed server objects compile", "[News]") {
    // Start the Tiingo News HTTP server for unit test
    HttpServer server;
    // Initialize the server would have been done through initServer function
    // For this stubbed test, we use the server handler directly
   
    SECTION( "Test a Tiingo News API Get Request" )
    {
      Configuration cfg("tests/config.json");
      NewsRequest req(cfg);

      NewsResponse lResponse = req.fetch();

      REQUIRE( lResponse.getStatus() == 0 );
    }
}
