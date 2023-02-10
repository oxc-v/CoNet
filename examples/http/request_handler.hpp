/**
 * 解析请求 
 */

#pragma once

#include "request.hpp"
#include "response.hpp"

class RequestHandler
{
public:
    enum ResultType {
        good,
        bad,
        indeterminate,
    };

    RequestHandler();

    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;

    /**
     * @brief 解析请求
     * @param msg 消息体
     * @return 
     */
     ResultType ParseRequest(const std::string& msg);

    /**
     * @brief 处理请求
     */
    void HandleRequest(Response& resp, ResultType result);

private:
    /**
     * @brief 逐个解析字符
     * @param input 字符
     * @return 
     */
    ResultType Consume(char input);

    bool IsChar(int c);
    bool IsCtl(int c);
    bool IsTspecial(int c);
    bool IsDigit(int c);

private:
    enum State {
        method_start,
        method,
        uri,
        http_version_h,
        http_version_t_1,
        http_version_t_2,
        http_version_p,
        http_version_slash,
        http_version_major_start,
        http_version_major,
        http_version_minor_start,
        http_version_minor,
        expecting_newline_1,
        header_line_start,
        header_lws,
        header_name,
        space_before_header_value,
        header_value,
        expecting_newline_2,
        expecting_newline_3,
    } state_;

    Request request_;
};