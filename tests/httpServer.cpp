#include "httpServer.hpp"

#include <stdexcept>

HttpServer::HttpServer()
    : port(61938),
      baseUrl("http://127.0.0.1:" + std::to_string(port)),
      isRunning(false),
      startupComplete(false),
      listenFailed(false) {
    // The stubbed tests rely on the server being up as soon as the object
    // exists; Start() blocks until the listening socket is bound.
    Start();
}

HttpServer::~HttpServer() {
    Stop();
}

void HttpServer::Start() {
    if (isRunning.exchange(true)) {
        return; // Already running
    }

    {
        const std::lock_guard<std::mutex> lock(startupMutex);
        startupComplete = false;
        listenFailed = false;
    }

    serverThread = std::thread(&HttpServer::loop, this);

    // Barrier: wait until the event loop thread has bound the listening
    // socket, so callers can issue requests as soon as Start() returns.
    std::unique_lock<std::mutex> lock(startupMutex);
    startupCv.wait(lock, [this] { return startupComplete.load(); });
    lock.unlock();

    if (listenFailed.load()) {
        isRunning = false;
        if (serverThread.joinable()) {
            serverThread.join();
        }
        throw std::runtime_error("HttpServer failed to listen on " + baseUrl);
    }
}

void HttpServer::Stop() {
    if (!isRunning.exchange(false)) {
        return; // Not running
    }
    if (serverThread.joinable()) {
        serverThread.join();
    }
}

const std::string& HttpServer::GetBaseUrl() const {
    return baseUrl;
}

uint16_t HttpServer::GetPort() const {
    return port;
}

void HttpServer::loop() {
    // The mongoose manager is created, polled, and freed on this thread
    // only; mongoose is not thread-safe.
    mg_log_set(MG_LL_ERROR); // Keep test output clean; still report errors
    mg_mgr mgr{};
    mg_connection* conn = initServer(&mgr, OnRequestMongoose);

    {
        const std::lock_guard<std::mutex> lock(startupMutex);
        listenFailed = (conn == nullptr);
        startupComplete = true;
    }
    startupCv.notify_all();

    while (conn != nullptr && isRunning.load()) {
        mg_mgr_poll(&mgr, 10);
    }

    mg_mgr_free(&mgr);
}

mg_connection* HttpServer::initServer(mg_mgr* mgr, mg_event_handler_t event_handler) {
    // Based on: https://mongoose.ws/docs/api/http/#mg_http_listen
    mg_mgr_init(mgr);
    mg_connection* conn = mg_http_listen(mgr, baseUrl.c_str(), event_handler, this);
    if (conn == nullptr) {
        MG_ERROR(("Failed to listen on %s", baseUrl.c_str()));
    }
    return conn;
}

void HttpServer::acceptConnection(mg_connection* /*conn*/) {
    // Plain HTTP: nothing to do. (A future HttpsServer subclass would call
    // mg_tls_init() here, the same way cpr's test servers do.)
}

void HttpServer::OnRequestMongoose(mg_connection* conn, int ev, void* ev_data) {
    // fn_data is the HttpServer pointer passed to mg_http_listen(); accepted
    // connections inherit it from the listening connection.
    HttpServer* server = static_cast<HttpServer*>(conn->fn_data);
    if (server == nullptr) {
        return;
    }

    switch (ev) {
    case MG_EV_ACCEPT:
        server->acceptConnection(conn);
        break;
    case MG_EV_HTTP_MSG:
        server->OnRequest(conn, static_cast<mg_http_message*>(ev_data));
        break;
    default:
        break;
    }
}

void HttpServer::OnRequest(mg_connection* conn, mg_http_message* msg) {
    const std::string uri{msg->uri.buf, msg->uri.len};

    if (uri == "/tiingo/news") {
        OnRequestNewsGet(conn, msg);
    } else if (uri == "/") {
        OnRequestRoot(conn, msg);
    } else if (uri == "/temp_redirect.html") {
        OnRequestTempRedirect(conn, msg);
    } else {
        OnRequestNotFound(conn, msg);
    }
}

void HttpServer::OnRequestNewsGet(mg_connection* conn, mg_http_message* /*msg*/) {
    // Stub of the Tiingo News endpoint (section 2.2.2 of the Tiingo news API
    // documentation). GET https://api.tiingo.com/tiingo/news returns a flat
    // JSON array of article objects. The per-article fields are: id (int32),
    // title, url, description, publishedDate, crawlDate, source (strings)
    // and tickers, tags (string arrays).
    const std::string response = R"([
  {
    "id": 1,
    "title": "Apple Reports Strong Earnings",
    "url": "https://example.com/article/AAPL-123",
    "description": "Apple reported strong quarterly earnings with revenue exceeding expectations.",
    "publishedDate": "2026-01-15T12:30:00+00:00",
    "crawlDate": "2026-01-15T12:31:00+00:00",
    "source": "example.com",
    "tickers": ["AAPL"],
    "tags": ["earnings", "technology"]
  }
])";
    mg_http_reply(conn, 200, "Content-Type: application/json\r\n", "%s", response.c_str());
}

void HttpServer::OnRequestRoot(mg_connection* conn, mg_http_message* msg) {
    const std::string method{msg->method.buf, msg->method.len};
    if (method == "OPTIONS") {
        OnRequestOptions(conn, msg);
    } else {
        mg_http_reply(conn, 405, "Content-Type: text/plain\r\nAllow: OPTIONS\r\n", "%s", "Method Not Allowed\n");
    }
}

void HttpServer::OnRequestOptions(mg_connection* conn, mg_http_message* /*msg*/) {
    const char* headers =
        "Content-Type: text/plain\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Credentials: true\r\n"
        "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, PATCH, OPTIONS\r\n"
        "Access-Control-Max-Age: 3600\r\n";

    mg_http_reply(conn, 200, headers, "%s", "OK");
}

void HttpServer::OnRequestNotFound(mg_connection* conn, mg_http_message* /*msg*/) {
    mg_http_reply(conn, 404, "Content-Type: text/plain\r\n", "%s", "Not Found\n");
}

// Unit test handlers
void HttpServer::OnRequestTempRedirect(mg_connection* conn, mg_http_message* /*msg*/) {
    mg_http_reply(conn, 302, "Location: /tiingo/news\r\n", "%s", "Moved\n");
}
