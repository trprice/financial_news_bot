// Configuration.h
#pragma once
#include <vector>
#include <string>

class Configuration {
public:
    // Default filename; can be overridden via command-line
    Configuration(const std::string& filename = "config.json") : filename(filename) {}

    // Parse JSON into member variables
    void load();

    // Getters
    std::string getApiToken() const { return apiToken; }
    const std::vector<std::string>& getTickers() const { return tickers; }

private:
    std::string filename;
    std::string apiToken;
    std::vector<std::string> tickers;

    void parse();
};
