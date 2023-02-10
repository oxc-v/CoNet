#include "request_handler.hpp"

#include <fstream>
#include <iostream>

RequestHandler::RequestHandler()
    : state_(method_start)
{
}

RequestHandler::ResultType RequestHandler::ParseRequest(const std::string& msg)
{
    auto begin = msg.begin();
    auto end = msg.end();

    while (begin != end) {
        auto result = Consume(*begin++);
        if (result == good || result == bad) {
            return result;
        }
    }

    return indeterminate;
}

void RequestHandler::HandleRequest(Response& resp, ResultType result)
{
    resp.version = "HTTP/1.1";

    if (result == bad) {
        resp.status = "400 Bad Reques";
        resp.body = "Invalid Request";
        resp.headers.push_back({"Content-Type", "text/plain"});
        resp.headers.push_back({"Content-Length", std::to_string(resp.body.size())});
        return;
    }

    // 读取测试图片
    std::string png = "/home/oxc/image" + request_.uri;
    std::ifstream ifs(png, std::ios::binary);
    if (!ifs) {
        resp.status = "404 Not Found";
        resp.body = "File Not Found";
        resp.headers.push_back({"Content-Type", "text/plain"});
        resp.headers.push_back({"Content-Length", std::to_string(resp.body.size())});
        return;
    }
    ifs.seekg(0, std::ios::end);
    resp.body.resize(ifs.tellg());
    ifs.seekg(0, std::ios::beg);
    ifs.read(&resp.body[0], resp.body.size());

    resp.status = "200 OK";
    resp.headers.push_back({"Content-Length", std::to_string(resp.body.size())});
    resp.headers.push_back({"Content-Type", "image/png"});

    ifs.close();
}

RequestHandler::ResultType RequestHandler::Consume(char input)
{
    switch (state_) {
    case method_start:
        if (!IsChar(input) || IsCtl(input) || IsTspecial(input)) {
            return bad;
        } else {
            state_ = method;
            return indeterminate;
        }
    case method:
        if (input == ' ') {
            state_ = uri;
            return indeterminate;
        } else if (!IsChar(input) || IsCtl(input) || IsTspecial(input)) {
            return bad;
        } else {
            request_.method.push_back(input);
            return indeterminate;
        }
    case uri:
        if (input == ' ') {
            state_ = http_version_h;
            return indeterminate;
        } else if (IsCtl(input)) {
            return bad;
        } else {
            request_.uri.push_back(input);
            return indeterminate;
        }
    case http_version_h:
        if (input == 'H') {
            state_ = http_version_t_1;
            request_.version.push_back(input);
            return indeterminate;
        } else {
            return bad;
        }
    case http_version_t_1:
        if (input == 'T') {
            state_ = http_version_t_2;
            request_.version.push_back(input);
            return indeterminate;
        } else {
            return bad;
        }
    case http_version_t_2:
        if (input == 'T') {
            state_ = http_version_p;
            request_.version.push_back(input);
            return indeterminate;
        } else {
            return bad;
        }
    case http_version_p:
        if (input == 'P') {
            state_ = http_version_slash;
            request_.version.push_back(input);
            return indeterminate;
        } else {
            return bad;
        }
    case http_version_slash:
        if (input == '/') {
            state_ = http_version_major_start;
            request_.version.push_back(input);
            return indeterminate;
        } else {
            return bad;
        }
    case http_version_major_start:
        if (IsDigit(input)) {
            state_ = http_version_major;
            request_.version.push_back(input);
            return indeterminate;
        } else {
            return bad;
        }
    case http_version_major:
        if (input == '.') {
            state_ = http_version_minor_start;
            request_.version.push_back(input);
            return indeterminate;
        } else if (IsDigit(input)) {
            request_.version.push_back(input);
            return indeterminate;
        } else {
            return bad;
        }
    case http_version_minor_start:
        if (IsDigit(input)) {
            state_ = http_version_minor;
            request_.version.push_back(input);
            return indeterminate;
        } else {
            return bad;
        }
    case http_version_minor:
        if (input == '\r') {
            state_ = expecting_newline_1;
            return indeterminate;
        } else if (IsDigit(input)) {
            request_.version.push_back(input);
            return indeterminate;
        } else {
            return bad;
        }
    case expecting_newline_1:
        if (input == '\n') {
            state_ = header_line_start;
            return indeterminate;
        } else {
            return bad;
        }
    case header_line_start:
        if (input == '\r') {
            state_ = expecting_newline_3;
            return indeterminate;
        } else if (!request_.headers.empty() && (input == ' ' || input == '\t')) {
            state_ = header_lws;
            return indeterminate;
        } else if (!IsChar(input) || IsCtl(input) || IsTspecial(input)) {
            return bad;
        } else {
            request_.headers.push_back(Request::Header());
            request_.headers.back().name.push_back(input);
            state_ = header_name;
            return indeterminate;
        }
    case header_lws:
        if (input == '\r') {
            state_ = expecting_newline_2;
            return indeterminate;
        } else if (input == ' ' || input == '\t') {
            return indeterminate;
        } else if (IsCtl(input)) {
            return bad;
        } else {
            state_ = header_value;
            request_.headers.back().value.push_back(input);
            return indeterminate;
        }
    case header_name:
        if (input == ':') {
            state_ = space_before_header_value;
            return indeterminate;
        } else if (!IsChar(input) || IsCtl(input) || IsTspecial(input)) {
            return bad;
        } else {
            request_.headers.back().name.push_back(input);
            return indeterminate;
        }
    case space_before_header_value:
        if (input == ' ') {
            state_ = header_value;
            return indeterminate;
        } else {
            return bad;
        }
    case header_value:
        if (input == '\r') {
            state_ = expecting_newline_2;
            return indeterminate;
        } else if (IsCtl(input)) {
            return bad;
        } else {
            request_.headers.back().value.push_back(input);
            return indeterminate;
        }
    case expecting_newline_2:
        if (input == '\n') {
            state_ = header_line_start;
            return indeterminate;
        } else {
            return bad;
        }
    case expecting_newline_3:
        return (input == '\n') ? good : bad;
    default:
        return bad;
    }

    return indeterminate;
}

bool RequestHandler::IsChar(int c)
{
    return c >= 0 && c <= 127;
}

bool RequestHandler::IsCtl(int c)
{
    return (c >= 0 && c <= 31) || (c == 127);
}

bool RequestHandler::IsTspecial(int c)
{
    switch (c) {
    case '(': case ')': case '<': case '>': case '@':
    case ',': case ';': case ':': case '\\': case '"':
    case '/': case '[': case ']': case '?': case '=':
    case '{': case '}': case ' ': case '\t':
        return true;
    default:
        return false;
    }
}

bool RequestHandler::IsDigit(int c)
{
    return c >= '0' && c <= '9';
}

