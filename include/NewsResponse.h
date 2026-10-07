// NewsResponse.h
#pragma once
#include <vector>
#include <string>
#include "APIResponse.h"

struct Article {
    std::string title;
    std::string description;
    std::string url;
    std::string source;
    std::string pubDate;
    std::string ticker;
};

class NewsResponse : public APIResponse {
public:
    NewsResponse();
    explicit NewsResponse(const std::string& ticker,
                          const std::vector<Article>& articles) : mTicker(ticker), mArticles(articles) {}

    const std::vector<Article>& getArticles() const { return mArticles; }
    void setArticles(std::vector<Article> articles) { mArticles = articles; }

    // {"status": int, "ticker": string, "articles": [{title, description, url, source, pubDate, ticker}]}
    nlohmann::json GetResponse() const override;

    int getStatus() const { return mStatus; } // status = 0 on success
    void setStatus(int status) { mStatus = status; }

private:
    std::string mTicker;
    int mStatus = 0;               // 0 = success, non‑zero = error
    std::vector<Article> mArticles;
};
