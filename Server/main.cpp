#include <iostream>
#include "Server.h"

int main(int argc, char** argv) {
    if(argc != 2)
        throw std::logic_error("Arguments are incorrect. ");

    Server server(std::stoi(std::string(argv[1])));
    server.listenClients();
    return 0;
}
