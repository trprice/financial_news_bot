// NewsResponse.cpp
#include "NewsResponse.h"
#include <vector>
#include <sstream>

NewsResponse::NewsResponse(const std::string& ticker)
    : ticker(ticker), articles{} {}

NewsResponse::NewsResponse(const std::string& ticker,
                           const std::vector<Article>& articles)
    : ticker(ticker), articles(articles) {}

const std::vector<Article>& NewsResponse::getArticles() const {
    return articles;
}

int NewsResponse::getStatus() const {
    return status; // status is set to 0 on success internally
}
