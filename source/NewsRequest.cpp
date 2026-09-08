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
    std::string url = ENDPOINT + "?token=" + cfg.getApiToken();

    // Build query string (comma separated)
    if (!cfg.getTickers().empty()) {
        url += "&tickers=";

        const std::vector<std::string> tickers = cfg.getTickers();
        
        for (const auto& ticker : tickers)
        {
            url += ticker + ",";
        }
    }

    auto response = cpr::Get(cpr::Url{url}, cpr::Header{
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

bool NewsRequest::hasError() const {
    return false; // In fetch() we already validated HTTP & Tiingo status
}
