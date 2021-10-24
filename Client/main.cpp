#include <iostream>
#include <random>
#include "Client.h"

int main(int argc, char** argv) {
    if(argc != 2)
        throw std::logic_error("Program arguments are incorrect. ");

    Client client(std::stoi(std::string(argv[1])));
    client.connectTo();

    for(auto i = 0; i < 100; ++i) {
        auto value = rand();
        std::cout << "Sent " << value << std::endl;

        client.sendMessage(LengthWrapper(value));
    }

    client.shutdown();
    return 0;
}
