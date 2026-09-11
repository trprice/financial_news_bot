// NewsRequest.cpp
#include "NewsRequest.h"
#include <iostream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <sstream>

NewsRequest::NewsRequest(const Configuration& cfg)
    : cfg(cfg) {}

NewsResponse NewsRequest::fetch() {
    const std::string ENDPOINT = "https://api.tiingo.com/tiingo/news";

    cpr::Parameters params{{"token", cfg.getApiToken()}};

    // Tiingo expects tickers as a single comma-separated parameter value
    const std::vector<std::string>& tickers = cfg.getTickers();
    if (!tickers.empty()) {
        std::string tickerList;
        for (const auto& ticker : tickers)
        {
            if (&ticker != &tickers.front())
            {
                tickerList += ",";
            }
            tickerList += ticker;
        }
        params.Add(cpr::Parameter{"tickers", tickerList});
    }

    cpr::Response response = cpr::Get(cpr::Url{ENDPOINT}, params, cpr::Header{
        {"Accept", "application/json"}
    });

    if (response.status_code != 200) {
        throw std::runtime_error("HTTP request failed (" + std::to_string(response.status_code) + ").");
    }

    std::stringstream body(response.text);
    nlohmann::json jsonBody;
    body >> jsonBody;

    // Tiingo's /tiingo/news endpoint returns a flat JSON array of article
    // objects on success; there is no status/message/ticker/messages wrapper.
    if (!jsonBody.is_array()) {
        std::string message = jsonBody.contains("detail") ? jsonBody["detail"].dump() : jsonBody.dump();
        throw TiingoError("Tiingo API returned an unexpected response: " + message);
    }

    std::vector<Article> articles;
    articles.reserve(jsonBody.size());
    for (const auto& item : jsonBody) {
        Article article;
        article.title       = item.value("title", "");
        article.description = item.value("description", "");
        article.url         = item.value("url", "");
        article.source      = item.value("source", "");
        article.pubDate     = item.value("publishedDate", "");

        if (item.contains("tickers") && item["tickers"].is_array() && !item["tickers"].empty()) {
            article.ticker = item["tickers"].front().get<std::string>();
        }

        articles.push_back(std::move(article));
    }

    return NewsResponse(tickers.empty() ? std::string() : tickers.front(), articles);
}