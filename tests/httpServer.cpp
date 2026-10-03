#include "httpServer.hpp"
#include "catch2/catch_test_macros.hpp"
#include "Configuration.h"
#include "NewsRequest.h"
#include "NewsResponse.h"

std::string HttpServer::GetBaseUrl() {
    return "http://127.0.0.1:" + std::to_string(GetPort());
}

uint16_t HttpServer::GetPort() {
    return 61938;
}

mg_connection* HttpServer::initServer(mg_mgr* mgr, mg_event_handler_t event_handler) {
    mg_mgr_init(mgr);
    std::string port = std::to_string(GetPort());
    return mg_http_listen(mgr, GetBaseUrl().c_str(), event_handler, this);
}

void HttpServer::acceptConnection(mg_connection* /*conn*/) {}

void HttpServer::OnRequestNewsGet(mg_connection* conn, mg_http_message* msg) {
    // Tiingo News API handler - simplified for unit tests
    // Accepts path/query parameters (ticker, date, etc.) based on section 2.2.2
    std::string response = R"({
      "status": "success",
      "api_token": "test_token_123",
      "news_articles": [
        {
          "id": "1",
          "ticker": "AAPL",
          "title": "Apple Reports Strong Earnings",
          "published_date": "2026-01-15",
          "headline": "Apple Reports Strong Financials",
          "source": "Financial Times",
          "url": "https://example.com/article/AAPL-123",
          "content_preview": "Apple reported strong quarterly earnings with revenue exceeding expectations."
        }
      ],
      "timestamp": "2026-01-15T12:30:00Z",
      "request_id": "test-req-001",
      "valid": true
    })";
    mg_http_reply(conn, 200, "Content-Type: application/json\r\n", response.c_str());
}

void HttpServer::OnRequestRoot(mg_connection* conn, mg_http_message* msg) {
    if (std::string{msg->method.ptr, msg->method.len} == std::string{"OPTIONS"}) {
        OnRequestOptions(conn, msg);
    } else {
        mg_http_reply(conn, 405, "Method Not Allowed".c_str(), "Method Not Allowed".c_str());
    }
}

void HttpServer::OnRequestOptions(mg_connection* conn, mg_http_message* msg) {
    std::string headers =
        "Content-Type: text/plain\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Credentials: true\r\n"
        "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, PATCH, OPTIONS\r\n"
        "Access-Control-Max-Age: 3600\r\n";

    mg_http_reply(conn, 200, headers.c_str(), "OK".c_str());
}

void HttpServer::OnRequestNotFound(mg_connection* conn, mg_http_message* msg) {
    mg_http_reply(conn, 404, "Not Found".c_str(), "Not Found".c_str());
}

// Unit test handlers
void HttpServer::OnRequestTempRedirect(mg_connection* conn, mg_http_message* msg) {
    mg_http_reply(conn, 302, "Location: /news.html\r\n", "Moved\n");
}

void HttpServer::acceptConnection(mg_connection* /*conn*/) {}
