#include <iostream>
#include <string>

#include "server.hpp"

using namespace std;

int main(int argc, char* argv[])
{
    Server server("127.0.0.1", 8888);
    server.Start();

    return 0;
}