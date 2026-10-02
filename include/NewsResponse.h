// NewsResponse.h
#pragma once
#include <vector>
#include <string>

struct Article {
    std::string title;
    std::string description;
    std::string url;
    std::string source;
    std::string pubDate;
    std::string ticker;
};

class NewsResponse {
public:
    NewsResponse();
    explicit NewsResponse(const std::string& ticker,
                          const std::vector<Article>& articles) : mTicker(ticker), mArticles(articles) {}

    const std::vector<Article>& getArticles() const { return mArticles; }
    void setArticles(std::vector<Article> articles) { mArticles = articles; }

    int getStatus() const { return mStatus; } // status = 0 on success
    void setStatus(int status) { mStatus = status; }

private:
    std::string mTicker;
    int mStatus = 0;               // 0 = success, non‑zero = error
    std::vector<Article> mArticles;
};
