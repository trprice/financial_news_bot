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
    if (cfg.getTickers().empty()) {
        throw std::runtime_error("No tickers provided – Tiingo API requires at least one ticker.");
    }

    // Build query string (comma separated)
    std::string url = ENDPOINT + "?token=" + cfg.getApiToken();
    if (!cfg.getTickers().empty()) {
        url += "&tickers=" + cfg.getTickers()[0];
        // For multiple tickers, we would need to loop over them or use a batch API (not needed now)
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
