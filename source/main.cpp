// main.cpp

#include "Configuration.h"
#include "NewsRequest.h"
#include "SqliteHandler.h"
#include <iostream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

int main(int argc, char* argv[]) {
    try {
        // 1. Parse CLI for custom config file (default is "config.json")
        std::string configFile = "config.json";
        if (argc > 2 && std::string(argv[1]) == std::string("--config")) {
            configFile = argv[2];
        }

        // 2. Load configuration
        Configuration cfg(configFile);
        cfg.load();

        // 3. Set up SQLite handler (creates DB if missing)
        SqliteHandler db("financial_news.db");
        db.ensureDatabaseExists();          // equivalent to createSchema()
        db.createSchema();

        // 4. Fetch news from Tiingo
        NewsRequest newsReq(cfg);
        NewsResponse resp = [&]() {
            try {
                return newsReq.fetch();
            } catch (const TiingoError& ex) {
                throw std::runtime_error(std::string("Tiingo API returned an error: ") + ex.what());
            } catch (const nlohmann::json::exception& ex) {
                throw std::runtime_error(std::string("Failed to parse Tiingo response: ") + ex.what());
            } catch (const std::runtime_error& ex) {
                throw std::runtime_error(std::string("HTTP request to Tiingo failed: ") + ex.what());
            }
        }();

        // 5. Store articles
        db.bulkInsert(resp);
        std::cout << "News stored successfully." << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
