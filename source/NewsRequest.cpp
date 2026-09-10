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

    auto response = cpr::Get(cpr::Url{ENDPOINT}, params, cpr::Header{
        {"Accept", "application/json"}
    });

    if (!response.is_success) {
        throw std::runtime_error("HTTP request failed (" + std::to_string(response.status_code) + ").");
    }

    std::stringstream body(response.text);
    nlohmann::json j;
    body >> j;

    if (!j.contains("status") || j["status"] != 0) {
        throw TiingoError("Tiingo API returned error: " + std::to_string(j["status"]) +
                          " with message: " + j["message"]);
    }

    return NewsResponse(j["ticker"].get<std::string>(), j["messages"].get<std::vector<nlohmann::json>>());
}