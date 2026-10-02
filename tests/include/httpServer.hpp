#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include "mongoose.h"

// HttpServer class declaration for testing tier
class HttpServer {
public:
    // Base URL getter
    static std::string GetBaseUrl();

    // Port getter
    static uint16_t GetPort();

    // Server initialization
    static mg_connection* initServer(mg_mgr* mgr, mg_event_handler_t event_handler);

    // Connection acceptance
    static void acceptConnection(mg_connection* conn);

    // Core HTTP request handlers
    static void OnRequestNewsGet(mg_connection* conn, mg_http_message* msg);
    static void OnRequestRoot(mg_connection* conn, mg_http_message* msg);
    static void OnRequestOptions(mg_connection* conn, mg_http_message* msg);
    static void OnRequestNotFound(mg_connection* conn, mg_http_message* msg);
    static void OnRequestTempRedirect(mg_connection* conn, mg_http_message* msg);
};

#endif // HTTP_SERVER_HPP
