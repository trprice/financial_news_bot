// main.cpp

#include "Configuration.h"
#include "NewsRequest.h"
#include "SqliteHandler.h"
#include <iostream>
#include <cpr/cpr.h>

int main(int argc, char* argv[]) {
    try {
        // 1. Parse CLI for custom config file (default is "config.json")
        std::string configFile = "config.json";
        if (argc > 1 && std::string(argv[1]) == std::string("--config")) {
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
        NewsResponse resp = newsReq.fetch();

        // 5. Validate API success
        if (!newsReq.hasError()) {
            // 6. Store articles
            db.bulkInsert(resp);
            std::cout << "✅ News stored successfully." << std::endl;
        } else {
            throw std::runtime_error("Tiingo API request failed – see error details above.");
        }
    } catch (const std::exception& ex) {
        std::cerr << "❌ Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
