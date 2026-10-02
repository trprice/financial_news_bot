// Configuration.cpp
#include "Configuration.h"
#include <fstream>
#include <iostream>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

void Configuration::load() {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        throw std::runtime_error("Failed to open configuration file: " + filename);
    }

    json j;
    inFile >> j;

    // Ensure that the endpoint and the token exist.
    //     The name of the api token config key will need to change when we support more than Tiingo.
    //     If there are APIs that don't require a token we'll need to change the structurte of the configuration
    //       so that it's only required for the APIs that need it.
    if ((!j.contains("tiingo_token")) || (!j.contains("endpoint"))) {
        throw std::runtime_error("Configuration must contain 'tiingo_token' (API key) and an endpoint");
    }
    apiToken = j["tiingo_token"];
    endPoint = j["endpoint"];

    // Optional tickers list
    if (j.contains("tickers")) {
        tickers = j["tickers"].get<std::vector<std::string>>();
    } else {
        tickers.clear();          // No tickers provided → empty list
    }

    inFile.close();
}
