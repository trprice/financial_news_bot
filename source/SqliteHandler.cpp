// SqliteHandler.cpp
#include "SqliteHandler.h"
#include <sqlite3.h>
#include <stdexcept>
#include <sstream>
#include <vector>

std::string SqliteHandler::dbPath;

SqliteHandler::SqliteHandler(const std::string& dbPath)
    : dbPath(dbPath) {}

void SqliteHandler::ensureDatabaseExists() {
    sqlite3* db;
    int rc = sqlite3_open_v2(dbPath.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
    if (rc) {
        throw std::runtime_error("Failed to open SQLite DB: " + std::string(sqlite3_errmsg(db)));
    }

    std::string sql = R"(
        CREATE TABLE IF NOT EXISTS articles (
            id INTEGER PRIMARY KEY,
            ticker TEXT,
            title TEXT,
            description TEXT,
            url TEXT,
            pubDate TEXT
        );
    )";

    const char* stmt = sql.c_str();
    rc = sqlite3_exec(db, stmt, nullptr, nullptr, nullptr);
    if (rc) {
        throw std::runtime_error("Failed to create SQLite schema: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_close(db);
}

void SqliteHandler::createSchema() {
    ensureDatabaseExists();
}

void SqliteHandler::bulkInsert(NewsResponse response) {
    sqlite3* db;
    int rc = sqlite3_open_v2(dbPath.c_str(), &db, SQLITE_OPEN_READWRITE, nullptr);
    if (rc) {
        throw std::runtime_error("Failed to open SQLite DB: " + std::string(sqlite3_errmsg(db)));
    }

    std::vector<std::string> toInsert;
    // Prepare a batch insert (limit per SQLite recommendation ~100 rows per transaction)
    const int BATCH_SIZE = 100;

    const std::vector<Article>& articles = response.getArticles();

    for (const auto& article : articles) {
        std::ostringstream oss;
        oss << "INSERT INTO articles (ticker, title, description, url, pubDate) "
                << "VALUES (?, ?, ?, ?, ?)";
        const char* insertStmt = oss.str().c_str();

        std::vector<void*> bindArgs[5] = {
            (void*)article.ticker.c_str(),
            (void*)article.title.c_str(),
            (void*)article.description.c_str(),
            (void*)article.url.c_str(),
            (void*)article.pubDate.c_str()
        };

        rc = sqlite3_exec(db, insertStmt, nullptr, bindArgs, nullptr);
        if (rc) {
            throw std::runtime_error("SQLite error inserting row: " + std::string(sqlite3_errmsg(db)));
        }

        // Track batch size
        if (++toInsert.size() >= BATCH_SIZE) {
            // Commit and reset
            rc = sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr);
            if (rc) {
                throw std::runtime_error("Failed to commit transaction: " + std::string(sqlite3_errmsg(db)));
            }
            toInsert.clear();
        }
    }

    // Final commit if any rows remain
    if (!toInsert.empty()) {
        rc = sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr);
        if (rc) {
            throw std::runtime_error("Final commit failed: " + std::string(sqlite3_errmsg(db)));
        }
    }

    sqlite3_close(db);
}
