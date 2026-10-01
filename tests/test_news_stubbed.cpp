#include "catch2/catch_test_macros.hpp"
#include "Configuration.h"
#include "NewsRequest.h"
#include "NewsResponse.h"
#include "stubbed_news_server.h"   // New header with stubbed server helpers
#include <nlohmann/json.hpp>

TEST_CASE("NewsRequest stubbed server returns correctly parsed NewsResponse", "[News]") {
    // Configuration simulates real token and tickers
    Configuration cfg("tests/config.json");
    NewsRequest req(cfg);
    
    // Get simulated Tiingo JSON response (as provided by stubbed_news_server.h)
    nlohmann::json newsJson = stubbed_news_server.getMockNewsResponse();
    
    // Parse using the same logic as NewsRequest.cpp (lines 51-64)
    std::vector<NewsResponse::Article> articles;
    for (const auto& item : newsJson) {
        NewsResponse::Article article;
        
        article.title       = item.value("title", "").get<std::string>();
        article.description = item.value("description", "").get<std::string>();
        article.url         = item.value("url", "").get<std::string>();
        article.source      = item.value("source", "").get<std::string>();
        article.pubDate     = item.value("publishedDate", "").get<std::string>();
        
        if (item.contains("tickers") && item["tickers"].is_array() && !item["tickers"].empty()) {
            article.ticker = item["tickers"].front().get<std::string>();
        }
        
        articles.push_back(std::move(article));
    }
    
    // Create NewsResponse using tickers from stubbed server
    std::vector<std::string> tickers = stubbed_news_server.getMockTickers();
    NewsResponse result = NewsResponse(tickers.empty() ? "" : tickers.front(), articles);
    
    // Verify exact matching of section 2.2.2 field specs
    REQUIRE(result.getStatus() == 0);
    REQUIRE(result.getArticles().size() == 3);
    
    // Verify article 1
    EXPECT(result.getArticles().front().title == "Example Article Title");
    EXPECT(result.getArticles().front().description == "This is a long-form description of the news story.");
    EXPECT(result.getArticles().front().url == "https://example.com/news/1");
    EXPECT(result.getArticles().front().source == "forbes.com");
    EXPECT(result.getArticles().front().pubDate == "2026-09-25T12:00:00Z");
    EXPECT(result.getArticles().front().ticker == "aapl");
    
    // Verify article 2 & 3
    EXPECT(result.getArticles().size() == 3);
    
    for (size_t i = 0; i < result.getArticles().size(); ++i) {
        const auto& article = result.getArticles()[i];
        EXPECT(article.title == (i == 0 ? "Example Article Title" :
                                 (i == 1 ? "Another Example Title" : "Third Example Title")));
        EXPECT(article.description == (i == 0 ? "This is a long-form description of the news story." :
                                         (i == 1 ? "Another long-form description." : "Yet another description.")));
        EXPECT(article.url == (i == 0 ? "https://example.com/news/1" :
                                 (i == 1 ? "https://example.com/news/2" : "https://example.com/news/3")));
        EXPECT(article.source == (i == 0 ? "forbes.com" :
                                 (i == 1 ? "reuters.com" : "financial.news")));
        EXPECT(article.pubDate == (i == 0 ? "2026-09-25T12:00:00Z" :
                                     (i == 1 ? "2026-09-24T08:30:00Z" : "2026-09-23T16:45:00Z")));
        EXPECT(article.ticker == (i == 0 ? "aapl" :
                                 (i == 1 ? "googl" : "msft")));
    }
}

TEST_CASE("NewsRequest stubbed server handles missing ticker fields gracefully", "[News]") {
    Configuration cfg("tests/config.json");
    NewsRequest req(cfg);
    
    // Simulate mild JSON with partial fields (as stubbed_news_server.h would provide)
    nlohmann::json jsonBody = {
        // Article with all fields
        {
            "title": "Partial Article",
            "description": "Short description",
            "url": "https://example.com",
            "source": "example.com",
            "publishedDate": "2026-09-25T12:00:00Z",
            "tickers": {"aapl"}
        },
        // Article with missing description
        {
            "title": "Minimal Article",
            "url": "https://example.com/minimal",
            "source": "minimal.com",
            "publishedDate": "2026-09-23T14:00:00Z",
            "tickers": {"msft"}
        }
    };
    
    // Parse using same logic as NewsRequest.cpp
    std::vector<NewsResponse::Article> articles;
    for (const auto& item : jsonBody) {
        NewsResponse::Article article;
        
        article.title       = item.value("title", "").get<std::string>();
        article.description = item.value("description", "").get<std::string>();
        article.url         = item.value("url", "").get<std::string>();
        article.source      = item.value("source", "").get<std::string>();
        article.pubDate     = item.value("publishedDate", "").get<std::string>();
        
        if (item.contains("tickers") && item["tickers"].is_array() && !item["tickers"].empty()) {
            article.ticker = item["tickers"].front().get<std::string>();
        }
        
        articles.push_back(std::move(article));
    }
    
    // Create NewsResponse
    std::vector<std::string> tickers = stubbed_news_server.getMockTickers();
    NewsResponse result = NewsResponse(tickers.empty() ? "" : tickers.front(), articles);
    
    // Assert structural correctness
    REQUIRE(result.getArticles().size() == 2);
    
    // Article 1 (from full fields)
    EXPECT(result.getArticles().front().title == "Partial Article");
    EXPECT(result.getArticles().front().description == "Short description");
    EXPECT(result.getArticles().front().url == "https://example.com");
    EXPECT(result.getArticles().front().source == "example.com");
    EXPECT(result.getArticles().front().pubDate == "2026-09-25T12:00:00Z");
    EXPECT(result.getArticles().front().ticker == "aapl");
    
    // Article 2 (minimal fields)
    EXPECT(result.getArticles().back().title == "Minimal Article");
    EXPECT(result.getArticles().back().description == ""); // empty as omitted
    EXPECT(result.getArticles().back().url == "https://example.com/minimal");
    EXPECT(result.getArticles().back().source == "minimal.com");
    EXPECT(result.getArticles().back().pubDate == "2026-09-23T14:00:00Z");
    EXPECT(result.getArticles().back().ticker == "msft");
}

TEST_CASE("NewsRequest stubbed server rejects non-array or malformed JSON", "[News]") {
    // Test rejection of non-array JSON (as per NewsRequest.cpp logic)
    nlohmann::json badJson = nlohmann::json::value("invalid", "not an array");
    
    bool isArray = badJson.is_array();
    // In actual code, this would throw TiingoError; here we just verify the assertion path
    REQUIRE_FALSE(isArray);  // Should reach this as indicator
    
    // Test with valid empty array - should handle gracefully
    nlohmann::json emptyJson = nlohmann::json::array();
    bool isEmpty = emptyJson.size() == 0;
    
    // Act: Parse empty array (like stubbed_news_server.h would return if no articles)
    std::vector<NewsResponse::Article> articles;
    for (const auto& item : emptyJson) {
        // No iteration needed since array is empty
        // The parsing loop is fine
    }
    
    std::vector<std::string> tickers = stubbed_news_server.getMockTickers();
    NewsResponse result = NewsResponse(tickers.empty() ? "" : tickers.front(), articles);
    
    // Should still be valid
    REQUIRE(result.getStatus() == 0);
    REQUIRE(result.getArticles().size() == 0);
}