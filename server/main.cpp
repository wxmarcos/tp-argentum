#include <iostream>
#include <stdexcept>
#include "server.h"


int main(int argc, char const *argv[])
{
    if (argc != 2) return 1;

    std::string port = argv[1];

    Server server(port.c_str());
    server.start();

    return 0;
}
