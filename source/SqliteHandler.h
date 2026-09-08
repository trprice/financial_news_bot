// SqliteHandler.h
#pragma once
#include <string>
#include <sqlite3.h>
#include "NewsResponse.h"

class SqliteHandler {
public:
    explicit SqliteHandler(const std::string& dbPath);
    void ensureDatabaseExists();
    void createSchema();
    void bulkInsert(NewsResponse);
};
