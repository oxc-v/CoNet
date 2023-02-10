/**
 * 响应体
 */

#pragma once

#include <string>
#include <vector>

struct Response
{
    struct Header
    {
        std::string name;
        std::string value;
    };

    std::string version;
    std::string status;
    std::vector<Header> headers;
    std::string body;

    std::string ToString() const
    {
        std::string result;
        result += version + " " + status + "\r\n";
        for (auto& header : headers) {
            result += header.name + ": " + header.value + "\r\n";
        }
        result += "\r\n";
        result += body;
        return result;
    }
};