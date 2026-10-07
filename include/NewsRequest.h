// NewsRequest.h
#pragma once
#include <stdexcept>
#include "APIRequest.h"
#include "Configuration.h"
#include "NewsResponse.h"
#include <memory>
#include <string>

class TiingoError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class NewsRequest : public APIRequest {
public:
    explicit NewsRequest(const Configuration& cfg);
    // GET /tiingo/news, typed for callers that need the NewsResponse API
    NewsResponse fetchNews();

    // GET /tiingo/news; the returned object is a NewsResponse
    std::unique_ptr<APIResponse> fetch() override;

    // The Tiingo news API has no POST endpoint; always throws std::logic_error
    std::unique_ptr<APIResponse> send() override;

private:
    Configuration cfg;
};
