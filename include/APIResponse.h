// APIResponse.h
#pragma once
#include <nlohmann/json.hpp>

// Base class for the result of an API call. Derived classes represent the
// response of a specific API.
class APIResponse {
public:
    APIResponse() = default;
    virtual ~APIResponse() = default;

    APIResponse(const APIResponse&) = default;
    APIResponse& operator=(const APIResponse&) = default;
    APIResponse(APIResponse&&) = default;
    APIResponse& operator=(APIResponse&&) = default;

    // The response of the API call as a json object.
    virtual nlohmann::json GetResponse() const = 0;
};
