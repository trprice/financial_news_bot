// NewsRequest.h
#pragma once
#include "Configuration.h"
#include <string>

class TiingoError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class NewsRequest {
public:
    explicit NewsRequest(const Configuration& cfg);
    NewsResponse fetch();
    bool hasError() const;

private:
    Configuration cfg;
};
