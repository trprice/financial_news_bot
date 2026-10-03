#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include "mongoose.h"
#include <string>

// HttpServer class declaration for testing tier
class HttpServer {
public:
    // Base URL getter
    std::string GetBaseUrl();

    // Port getter
    uint16_t GetPort();

    // Server initialization
    mg_connection* initServer(mg_mgr* mgr, mg_event_handler_t event_handler);

    // Connection acceptance
    void acceptConnection(mg_connection* conn);

private:
    // Core HTTP request handlers
    static void OnRequestNewsGet(mg_connection* conn, mg_http_message* msg);
    static void OnRequestRoot(mg_connection* conn, mg_http_message* msg);
    static void OnRequestOptions(mg_connection* conn, mg_http_message* msg);
    static void OnRequestNotFound(mg_connection* conn, mg_http_message* msg);
    static void OnRequestTempRedirect(mg_connection* conn, mg_http_message* msg);
};

#endif // HTTP_SERVER_HPP
