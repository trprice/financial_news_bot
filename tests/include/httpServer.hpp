#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include "mongoose.h"

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <string>
#include <thread>

// HttpServer class declaration for the testing tier.
//
// Modeled after the HTTP server that cpr uses for its own tests: a real
// mongoose server that runs its event loop on a background thread, so tests
// can exercise the full request path (NewsRequest -> cpr -> TCP -> stub).
class HttpServer {
public:
    // Constructs the server and starts it. Start() blocks until the listening
    // socket is bound, so requests may be made as soon as construction
    // returns. Throws std::runtime_error if the port cannot be bound.
    HttpServer();
    ~HttpServer();

    HttpServer(const HttpServer&) = delete;
    HttpServer& operator=(const HttpServer&) = delete;
    HttpServer(HttpServer&&) = delete;
    HttpServer& operator=(HttpServer&&) = delete;

    // Start the event loop thread. Idempotent.
    void Start();

    // Stop the event loop thread and free the mongoose manager. Idempotent.
    void Stop();

    // Base URL getter
    const std::string& GetBaseUrl() const;

    // Port getter
    uint16_t GetPort() const;

    // Server initialization; called on the event loop thread
    mg_connection* initServer(mg_mgr* mgr, mg_event_handler_t event_handler);

    // Connection acceptance; called from the event loop on MG_EV_ACCEPT
    void acceptConnection(mg_connection* conn);

private:
    // Background thread body: initialize the server, poll until stopped,
    // then free the mongoose manager
    void loop();

    // mongoose event handler; dispatches events for the server connection
    static void OnRequestMongoose(mg_connection* conn, int ev, void* ev_data);

    // Routes a parsed HTTP request to the matching handler by URI
    void OnRequest(mg_connection* conn, mg_http_message* msg);

    // Core HTTP request handlers
    static void OnRequestNewsGet(mg_connection* conn, mg_http_message* msg);
    static void OnRequestRoot(mg_connection* conn, mg_http_message* msg);
    static void OnRequestOptions(mg_connection* conn, mg_http_message* msg);
    static void OnRequestNotFound(mg_connection* conn, mg_http_message* msg);
    static void OnRequestTempRedirect(mg_connection* conn, mg_http_message* msg);

    uint16_t port;
    std::string baseUrl;
    std::atomic<bool> isRunning;
    std::atomic<bool> startupComplete;
    std::atomic<bool> listenFailed;
    std::thread serverThread;
    std::mutex startupMutex;
    std::condition_variable startupCv;
};

#endif // HTTP_SERVER_HPP
