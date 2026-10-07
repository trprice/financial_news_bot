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

      NewsResponse lResponse = req.fetchNews();

      REQUIRE( lResponse.getStatus() == 0 );

      std::vector<Article> lArticles = lResponse.getArticles();

      // At the moment the httpServer is hard coded to send back a single article.
      // Change this requirement (or make a new test with a different requirement) if we add an endpoint that returns more.
      REQUIRE( lArticles.size() == 1 );

      /***
       * This is the json that is returned by the httpServer. It highlights that our current implementation of struct Article is not complete
      {
        "id": 1,
        "title": "Apple Reports Strong Earnings",
        "url": "https://example.com/article/AAPL-123",
        "description": "Apple reported strong quarterly earnings with revenue exceeding expectations.",
        "publishedDate": "2026-01-15T12:30:00+00:00",
        "crawlDate": "2026-01-15T12:31:00+00:00",
        "source": "example.com",
        "tickers": ["AAPL"],
        "tags": ["earnings", "technology"]
      }
    ])";
    ***/
      REQUIRE( lArticles[0].title.compare("Apple Reports Strong Earnings") == 0 );
      REQUIRE( lArticles[0].url.compare("https://example.com/article/AAPL-123") == 0 );
      REQUIRE( lArticles[0].description.compare("Apple reported strong quarterly earnings with revenue exceeding expectations.") == 0 );
      REQUIRE( lArticles[0].pubDate.compare("2026-01-15T12:30:00+00:00") == 0 );
      REQUIRE( lArticles[0].source.compare("example.com") == 0 );
      REQUIRE( lArticles[0].ticker.compare("AAPL") == 0 );
    }

    SECTION( "fetch() through the APIRequest interface returns the articles as json" )
    {
      Configuration cfg("tests/config.json");
      NewsRequest req(cfg);
      APIRequest& api = req;

      std::unique_ptr<APIResponse> lResponse = api.fetch();
      nlohmann::json lJson = lResponse->GetResponse();

      REQUIRE( lJson["status"] == 0 );
      REQUIRE( lJson["articles"].size() == 1 );
      REQUIRE( lJson["articles"][0]["title"] == "Apple Reports Strong Earnings" );
      REQUIRE( lJson["articles"][0]["ticker"] == "AAPL" );
    }
}
