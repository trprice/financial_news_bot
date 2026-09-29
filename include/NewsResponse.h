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
    NewsResponse(const std::string& ticker);
    explicit NewsResponse(const std::string& ticker,
                          const std::vector<Article>& articles) : ticker(ticker), articles(articles) {}

    const std::vector<Article>& getArticles() const { return articles; }
    int getStatus() const { return status; } // status = 0 on success

private:
    std::string ticker;
    int status = 0;               // 0 = success, non‑zero = error
    std::vector<Article> articles;
};
