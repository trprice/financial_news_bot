// NewsResponse.cpp
#include "NewsResponse.h"
#include <vector>
#include <sstream>

NewsResponse::NewsResponse()
    : mTicker(), mArticles{} {}


nlohmann::json NewsResponse::GetResponse() const {
    nlohmann::json articles = nlohmann::json::array();
    for (const auto& article : mArticles) {
        articles.push_back({{"title", article.title},
                            {"description", article.description},
                            {"url", article.url},
                            {"source", article.source},
                            {"pubDate", article.pubDate},
                            {"ticker", article.ticker}});
    }
    return {{"status", mStatus}, {"ticker", mTicker}, {"articles", articles}};
}
