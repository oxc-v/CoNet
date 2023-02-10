#include "server.hpp"

#include <csignal>
#include <iostream>

Server::Server(const std::string& ip, int port, std::size_t loopSize)
    : loopPool_(loopSize), 
      signals_(loopPool_.GetNextLoop()),  
      acceptor_(loopPool_.GetNextLoop(), conet::ip::Endpoint(conet::ip::Address::FromString(ip), port))
{
    // 注册信号处理函数
    signals_.Add(SIGINT);
    signals_.Add(SIGTERM);
    signals_.Add(SIGQUIT);
    HandleSignals();

    // 处理连接
    acceptor_.SetReuseAddress(true);
    acceptor_.Bind();
    acceptor_.Listen();
    HandleConnection();
}

conet::awaitable::Awaitable Server::HandleSignals()
{
    conet::error::error_code ec;
    co_await signals_.AsyncWait(ec);
    loopPool_.Stop();
}

conet::awaitable::Awaitable Server::HandleConnection()
{
    // 无限循环，处理连接
    for (;;) {
        conn_.reset(new Connection(loopPool_.GetNextLoop()));
        conet::error::error_code ec;
        co_await acceptor_.AsyncAccept(conn_->Socket(), ec);

        if (!ec) {
            conn_->Start();
        } else {
            std::cerr << "accept error: " << ec.message() << std::endl;
            break;
        }
    }
}

void Server::Start()
{
    loopPool_.Start();
}

Server::~Server()
{
    loopPool_.Stop();
}