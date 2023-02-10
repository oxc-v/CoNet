#include "connection.hpp"

#include "request_handler.hpp"

const int MAXBUFSIZE = 1024;

Connection::Connection(conet::EventLoop& loop)
    : sock_(loop)
{
}

conet::tcp::Socket& Connection::Socket() noexcept
{
    return sock_;
}

void Connection::Start()
{
    HandleRead();
}

void Connection::Stop()
{
    sock_.Close();
}

conet::awaitable::Awaitable Connection::HandleRead()
{
    RequestHandler handler;
    Response response;

    for (;;) {
        conet::error::error_code ec;
        std::string msg;
        co_await sock_.AsyncReadSome(msg, MAXBUFSIZE, ec);
        if (!ec) {
            auto ret = handler.ParseRequest(msg);
            if (ret == RequestHandler::good || ret == RequestHandler::bad) {
                handler.HandleRequest(response, ret);
                break;
            }
        } else {
            std::cerr << "read error: " << ec.message() << std::endl;
            break;
        }
    }

    HandleWrite(response);
}

conet::awaitable::Awaitable Connection::HandleWrite(const Response& resp)
{
    conet::error::error_code ec;
    auto str = resp.ToString();
    co_await sock_.AsyncWrite(str, str.size(), ec);
    Stop();
}