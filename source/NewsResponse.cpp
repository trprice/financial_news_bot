// NewsResponse.cpp
#include "NewsResponse.h"
#include <vector>
#include <sstream>

NewsResponse::NewsResponse(const std::string& ticker)
    : ticker(ticker), articles{} {}

