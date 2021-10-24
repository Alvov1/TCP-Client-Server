#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h> // Директива линковщику: использовать библиотеку сокетов
    #pragma comment(lib, "ws2_32.lib")
#else // LINUX
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <cerrno>
    #include <unistd.h>
#endif

#include <iostream>
#include <string>
#include <cstring>
#include "../LengthWrapper.h"

class Client {
    int socket_;
    struct sockaddr_in addr{};

    int init();
    void deInit();
    void closeSocket() const;

    bool connected = false;
public:
    Client(unsigned Port, const std::string& IP = "127.0.0.1");
    /* If IP address is not specified, loopback address is used instead. */

    Client& operator=(const Client& client) = delete;
    ~Client();

    void connectTo();
    int sendMessage(const LengthWrapper& value) const;
    void shutdown() const;
};

unsigned int getHostIpn(const char* name);
#endif //CLIENT_CLIENT_H