/**
 * 请求体 
 */

#pragma once

#include <string>
#include <vector>

struct Request
{
    struct Header
    {
        std::string name;
        std::string value;
    };

    std::string method;
    std::string uri;
    std::string version;
    std::vector<Header> headers;
    std::string body;
};