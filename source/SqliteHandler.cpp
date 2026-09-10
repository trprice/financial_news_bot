// SqliteHandler.cpp
#include "SqliteHandler.h"
#include <sqlite3.h>
#include <stdexcept>
#include <vector>

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

    const char* insertSql =
        "INSERT INTO articles (ticker, title, description, url, pubDate) "
        "VALUES (?, ?, ?, ?, ?)";

    sqlite3_stmt* stmt = nullptr;
    rc = sqlite3_prepare_v2(db, insertSql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::string err = sqlite3_errmsg(db);
        sqlite3_close(db);
        throw std::runtime_error("Failed to prepare insert statement: " + err);
    }

    // Batch commits (limit per SQLite recommendation ~100 rows per transaction)
    const int BATCH_SIZE = 100;
    int rowsInBatch = 0;

    sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);

    const std::vector<Article>& articles = response.getArticles();

    for (const auto& article : articles) {
        sqlite3_bind_text(stmt, 1, article.ticker.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, article.title.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, article.description.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, article.url.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, article.pubDate.c_str(), -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            std::string err = sqlite3_errmsg(db);
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            throw std::runtime_error("SQLite error inserting row: " + err);
        }

        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);

        if (++rowsInBatch >= BATCH_SIZE) {
            rc = sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr);
            if (rc) {
                std::string err = sqlite3_errmsg(db);
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                throw std::runtime_error("Failed to commit transaction: " + err);
            }
            sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);
            rowsInBatch = 0;
        }
    }

    // Final commit for any remaining rows in the last partial batch
    rc = sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr);
    if (rc) {
        std::string err = sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        throw std::runtime_error("Final commit failed: " + err);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}
