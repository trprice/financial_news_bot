#pragma once

// StubbedNewsServer provides mock Tiingo API responses for unit testing.
// It simulates the JSON structure returned by https://api.tiingo.com/tiingo/news
// without requiring a real HTTP server.

#include <nlohmann/json.hpp>
#include <vector>
#include <string>

class StubbedNewsServer {
public:
    // Returns the mock tickers configuration (e.g., {"aapl", "googl", "msft"})
    static std::vector<std::string> getMockTickers() {
        return {
            "aapl",
            "googl", 
            "msft"
        };
    }

    // Returns a mock cpr::Response as if the stubbed server served the endpoint
    // For test integration, this is meant to be used by cpr's Test harness or simplified
    // Here we provide a simple function that returns the parsed JSON response
    // matching section 2.2.2 (as seen in the Tiingo News API docs).
    // This is the JSON structure used by NewsRequest::fetch() in source/NewsRequest.cpp.
    static nlohmann::json getMockNewsResponse() {
        // Tiingo's news endpoint returns a JSON array of article objects
        // matching the fields defined in section 2.2.2
        
        // Example: 3 articles as defined in the Tiingo docs section
        return nlohmann::json::dump(
            nlohmann::json::array(),
            /* structure */
            [/* 3-article array */] {
                // Article 1
                {
                    "title": "Example Article Title",
                    "description": "This is a long-form description of the news story.",
                    "url": "https://example.com/news/1",
                    "source": "forbes.com",
                    "publishedDate": "2026-09-25T12:00:00Z",
                    "tickers": {"aapl"}
                },
                // Article 2
                {
                    "title": "Another Example Title",
                    "description": "Another long-form description.",
                    "url": "https://example.com/news/2",
                    "source": "reuters.com",
                    "publishedDate": "2026-09-24T08:30:00Z",
                    "tickers": {"googl"}
                },
                // Article 3
                {
                    "title": "Third Example Title",
                    "description": "Yet another description.",
                    "url": "https://example.com/news/3",
                    "source": "financial.news",
                    "publishedDate": "2026-09-23T16:45:00Z",
                    "tickers": {"msft"}
                }
            }
        );
    }

    // Returns the parsed NewsResponse from the stubbed JSON (like NewsRequest.cpp does)
    // This mirrors the exact parsing logic in source/NewsRequest.cpp lines 51-64.
    static std::vector<NewsResponse::Article> parseMockNewsResponse(nlohmann::json jsonBody) {
        std::vector<NewsResponse::Article> articles;
        articles.reserve(jsonBody.size());
        for (const auto& item : jsonBody) {
            NewsResponse::Article article;
            
            // These are the direct JSON keys from Tiingo (as per section 2.2.2)
            article.title       = item.value("title", "").get<std::string>();
            article.description = item.value("description", "").get<std::string>();
            article.url         = item.value("url", "").get<std::string>();
            article.source      = item.value("source", "").get<std::string>();
            article.pubDate     = item.value("publishedDate", "").get<std::string>();
            
            // Handle tickers if present
            if (item.contains("tickers") && item["tickers"].is_array() && !item["tickers"].empty()) {
                article.ticker = item["tickers"].front().get<std::string>();
            }
            
            articles.push_back(std::move(article));
        }
        return articles;
    }
};

// Forward declaration for the Article struct if the test needs it
struct NewsResponse_Article {
    std::string title;
    std::string description;
    std::string url;
    std::string source;
    std::string pubDate;
    std::string ticker; // for tickers that are present
};