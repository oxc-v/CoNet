#include <csignal>
#include <iostream>

#include "conet.hpp"

using namespace std;

class EchoServer
{
public:
    EchoServer()
        : loop_(),
          acceptor_(loop_, conet::ip::Endpoint(2, 6666))
    {
        start();
    }

    conet::awaitable::Awaitable start()
    {
        acceptor_.SetReuseAddress(true);
        auto ret =  acceptor_.Bind();
        if (!ret) {
            cout << "bind error." << endl;
        }
        acceptor_.Listen();
        conet::tcp::Socket socket(loop_);
        conet::error::error_code ec;
        co_await acceptor_.AsyncAccept(socket, ec);
        if (!ec) {
            cout << "new conn." << endl;
            cout << "remote: " << socket.Remote().ToString() << endl;
        }
        while (true)
        {
            conet::error::error_code ec;
            std::string buf;
            auto n = co_await socket.AsyncRead(buf, 3, ec);
            if (ec) {
                cout << "read error: " << ec.message() << endl;
                if (ec != conet::error::would_block)
                    break;
            }
            cout << "data: " << buf << "   size: " << n << endl;

            conet::error::error_code ec2;
            auto m = co_await socket.AsyncWrite(buf, buf.size(), ec2);
            if (ec2) {
                cout << "write error: " << ec2.message() << endl;
                if (ec2 != conet::error::would_block)
                    break;
            }
            cout << "write size: " << m << endl;
        }
    }

    void run()
    {
        loop_.Run();
    }

    void stop()
    {
        loop_.Stop();
    }

private:
    conet::EventLoop loop_;
    conet::tcp::Acceptor acceptor_;

};

EchoServer server;
static void HandleSignal(int sig)
{
    cout << "signal: " << sig << endl;
    server.stop();
}

int main()
{
    std::signal(SIGINT, HandleSignal);

    server.run();
    std::cout << "exit." << std::endl;
    return 0;
}