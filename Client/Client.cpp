#include "Client.h"

Client::Client(unsigned Port, const std::string& IP) {
    init();
    socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_ < 0)
        throw std::logic_error("Error with openning the socket stream");

    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = getHostIpn(IP.c_str());
    addr.sin_port = htons(Port);
    addr.sin_family = AF_INET;
}
Client::~Client() {
    std::cout << "Disconnected. " << std::endl;
    closeSocket();
    deInit();
}

void Client::closeSocket() const {
#ifdef _WIN32
    closesocket(socket_);
#else
    close(socket_);
#endif
}
int Client::init() {
#ifdef _WIN32
    WSADATA wsa_data;
    return (0 == WSAStartup(MAKEWORD(2, 2), &wsa_data));
#else
    return 1;
#endif
}
void Client::deInit() {
#ifdef _WIN32
    WSACleanup();
#else
#endif
}

void Client::connectTo() {
    if(connected)
        throw std::logic_error("Client is already connected to the server. ");

    for (auto i = 0; i < 10; ++i)
        if (connect(socket_, (struct sockaddr *) &addr, sizeof(addr)) == 0) {
            std::cout << "Connected. " << std::endl;
            connected = true;
            return;
        }
    throw std::logic_error("Error with connection to the server");
}
void Client::shutdown() const {
    if(connected) {
        auto* sym = new unsigned char; /* Terminating message for the server. */
        *sym = 255;
        if(send(socket_, reinterpret_cast<char*>(sym), sizeof(char), 0) <= 0)
            std::cout << "Error with shutting down. " << std::endl;
    } else
        std::cout << "Client is not connected to the server. " << std::endl;
}
int Client::sendMessage(const LengthWrapper &value) const {
    if(!connected) {
        std::cout << "Client is not connected to the server" << std::endl;
        return -1;
    }

    char* ptr = value.data();
    const auto ptrLen = value.Length();

    auto sendCount = 0;
    for(; sendCount < ptrLen; ++sendCount)
        if(send(socket_, ptr + sendCount, sizeof(char), 0) <= 0) {
            std::cout << "Sending wasn't successful. " << std::endl;
            break;
        }

//    if(sendCount == ptrLen)
//        std::cout << "Sending was successful. " << std::endl;

    return sendCount;
}

unsigned int getHostIpn(const char* name) {
    struct addrinfo* addr = nullptr;
    unsigned int ip4addr = 0;
    if (0 == getaddrinfo(name, nullptr, nullptr, &addr)) {
        struct addrinfo* cur = addr;
        while (cur) {
            if (cur->ai_family == AF_INET) {
                ip4addr = ((struct sockaddr_in*) cur->ai_addr)->sin_addr.s_addr;
                break;
            }
            cur = cur->ai_next;
        }
        freeaddrinfo(addr);
    }
    return ip4addr;
}
