// Configuration.cpp
#include "Configuration.h"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

void Configuration::load() {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        throw std::runtime_error("Failed to open configuration file: " + filename);
    }

    json j;
    inFile >> j;

    // Ensure required token exists
    if (!j.contains("tiingo_token")) {
        throw std::runtime_error("Configuration must contain 'tiingo_token' (API key).");
    }
    apiToken = j["tiingo_token"];

    // Optional tickers list
    if (j.contains("tickers")) {
        tickers = j["tickers"].get<std::vector<std::string>>();
    } else {
        tickers.clear();          // No tickers provided → empty list
    }

    inFile.close();
}
