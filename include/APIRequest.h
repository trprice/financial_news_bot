// APIRequest.h
#pragma once
#include <memory>
#include "APIResponse.h"

// Base class for requests to a remote API. Derived classes represent a
// specific API: fetch() issues a GET request and send() issues a POST request.
// Responses are returned by pointer so that derived response types are not sliced.
class APIRequest {
public:
    APIRequest() = default;
    virtual ~APIRequest() = default;

    APIRequest(const APIRequest&) = default;
    APIRequest& operator=(const APIRequest&) = default;
    APIRequest(APIRequest&&) = default;
    APIRequest& operator=(APIRequest&&) = default;

    // Issue a GET request to the API.
    virtual std::unique_ptr<APIResponse> fetch() = 0;

    // Issue a POST request to the API.
    virtual std::unique_ptr<APIResponse> send() = 0;
};
