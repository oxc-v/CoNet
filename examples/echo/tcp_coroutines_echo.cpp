/**
 * 一个简单的tcp echo服务器 
 */

#include <csignal>
#include <iostream>
#include <memory>
#include <unordered_map>

#include "conet.hpp"

using namespace conet;
using namespace std;

class Session
{
public:
    Session(EventLoop& loop)
        : sock_(loop)
    {
    }

    ~Session()
    {
        cout << "Session deleted." << endl;
    }

    tcp::Socket& Socket() noexcept
    {
        return sock_;
    }

    awaitable::Awaitable Co_Start()
    {
        while (true) {
            // 读取数据
            error::error_code ec;
            auto n = co_await sock_.AsyncReadSome(msg_, MAXBUFSIZE, ec);
            if (ec) {
                if (ec != error::would_block) {
                    cout << "Read: " << ec.message() << endl;
                    break;
                } 
            }
            cout << "Receive: " << msg_ << " size: " << n << endl;

            // 写入数据
            error::error_code ec2;
            auto m = co_await sock_.AsyncWrite(msg_, msg_.size(), ec2);
            if (ec2) {
                if (ec2 != error::would_block) {
                    cout << "Write: " << ec2.message() << endl;
                    break;
                } 
            }
            cout << "Write size: " << m << endl;
        
            // 清空缓冲区
            msg_.clear();
        }
    }

private:
    tcp::Socket sock_;
    std::string msg_;
    enum { MAXBUFSIZE = 1024 };
};

class EchoServer
{
public:
    EchoServer()
        : loop_(),
          acceptor_(loop_, ip::Endpoint(2, 6666)),
          signals_(loop_)
    {
        acceptor_.SetReuseAddress(true);
        acceptor_.Bind();
        acceptor_.Listen();

        signals_.Add(SIGINT);
        signals_.Add(SIGTERM);
        Co_HandleSignals();

        // 启动一个协程监听连接，如果没有连接会立即返回
        Co_HandleAccept();
    }

    awaitable::Awaitable Co_HandleAccept()
    {
        while (true) {
            error::error_code ec;
            auto session = make_shared<Session>(loop_);
            co_await acceptor_.AsyncAccept(session->Socket(), ec);
            if (!ec) {
                cout << "New conn: " << session->Socket().Remote().ToString() << endl;

                sessions_.insert({ session->Socket().Fd(), session });

                // 监听连接关闭事件
                Co_HandleClosed(session->Socket());

                // 开始处理连接
                session->Co_Start();
            } else {
                cout << "Accept: " << ec.message() << endl;
                break;
            }
        }
    }

    awaitable::Awaitable Co_HandleClosed(tcp::Socket& sock)
    {
        error::error_code ec;
        co_await sock.AsyncWait(tcp::Socket::socket_closed, ec);
        if (!ec) {
            cout << "Connection closed: " << sock.Remote().ToString() << endl;

            sock.Close();
            sessions_.erase(sock.Fd());
        } else {
            cout << "AsyncWait: " << ec.message() << endl;
        }
    }

    awaitable::Awaitable Co_HandleSignals()
    {
        error::error_code ec;
        auto sig = co_await signals_.AsyncWait(ec);
        cout << "Signal: " << sig << endl;
        Stop();
    }

    void Run()
    {
        loop_.Run();
    }

    void Stop()
    {
        loop_.Stop();
    }

private:
    string msg_;
    EventLoop loop_;
    tcp::Acceptor acceptor_;
    signal::SignalSet signals_;
    unordered_map<int, shared_ptr<Session>> sessions_;
};

int main()
{
    EchoServer server;
    server.Run();

    cout << "Server exit." << endl;

    return 0;
}