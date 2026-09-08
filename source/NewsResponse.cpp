// NewsResponse.cpp
#include "NewsResponse.h"
#include <vector>
#include <sstream>

NewsResponse::NewsResponse(const std::string& ticker)
    : ticker(ticker), articles{} {}

NewsResponse::NewsResponse(const std::string& ticker,
                           const std::vector<Article>& articles)
    : ticker(ticker), articles(articles) {}

