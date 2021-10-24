#include <iostream>
#include <cstring>
#include "Server.h"
Server::Server(unsigned Port) : port(Port) {
    init();

    socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_ < 0)
        throw std::logic_error("Error with openning the socket stream");

    setNonBlock(socket_);

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(Port);

    for(auto i = 0; i < CLIENTS_LIMIT; ++i) {
        pfd[i].fd = cs[i];
        pfd[i].events = POLLIN | POLLOUT;
    }
    pfd[CLIENTS_LIMIT].fd = socket_;
    pfd[CLIENTS_LIMIT].events = POLLIN;

    if(bind(socket_, (struct sockaddr*) &address, sizeof(address)) < 0)
        throw std::logic_error("Bind server socket " + std::to_string(socket_));

    if(listen(socket_, 1) < 0)
        throw std::logic_error("Error in listening server socket " + std::to_string(socket_));

    createThreads();
}
Server::~Server() {
    std::cout << "Got " << messages.size() << " messages" << std::endl << "Terminating..." << std::endl;
    s_close(socket_);
    deinit();
}
void Server::listenClients() {
    messages.reserve(messagesLimit);
    auto listedMessages = 0;

    std::cout << "Listening" << std::endl;
    do {
        for(; listedMessages < messages.size(); ++listedMessages)
            std::cout << listedMessages + 1 << ". Message " << messages[listedMessages] << std::endl;

#ifdef _WIN32
        int ev_cnt = WSAPoll(pfd, sizeof(pfd) / sizeof(pfd[0]), 1000);
#else
        int ev_cnt = poll(pfd, sizeof(pfd) / sizeof(pfd[0]), 1000);
#endif
        if (ev_cnt > 0) {
            for (auto i = 0; i < CLIENTS_LIMIT; i++) {
                if (pfd[i].revents & POLLHUP) {
                    /* Client on socket cs[i] disconnected. Time to close socket. */
                    s_close(pfd[i].fd);
                    std::cout << "Client " << ipToString(ntohl(address.sin_addr.s_addr))
                              << ":" << port << " disconnected" << std::endl;
                }
                if (pfd[i].revents & POLLERR) {
                    /* Error on socket cs[i]. Time to close it. */
                    s_close(pfd[i].fd);
                    std::cout << "Client " << ipToString(ntohl(address.sin_addr.s_addr))
                              << ":" << port << " disconnected" << std::endl;
                }
                if (pfd[i].revents & POLLIN) {
                    /* Socket cs[i] is ready for reading. */
                    syncInfo.socketToListen = cs[i];
                    syncInfo.event.notify_one();
                }
            }
            if (pfd[CLIENTS_LIMIT].revents & POLLIN) {
                /* New connection. Socket can be added to the list. */
                socklen_t addrSize = sizeof(address);
                int socket = accept(socket_, (struct sockaddr*) &address, &addrSize);
                setNonBlock(socket);
                if(socket < 0)
                    throw std::logic_error("Accept error with socket " + std::to_string(socket_));

                cs[socketCount] = socket;
                pfd[socketCount].fd = socket;
                socketCount++;

                std::cout << "Client " << ipToString(ntohl(address.sin_addr.s_addr))
                          << ":" << port << " connected" << std::endl;
            }
        }
    } while (working);
}
void Server::createThreads() {
    threads.reserve(threadsNumber);

    for(auto tNumber = 0; tNumber < threadsNumber; ++tNumber)
        threads.emplace_back(threadEntry, &syncInfo, &working, &messages);

    for(auto tNumber = 0; tNumber < threadsNumber; ++tNumber)
        threads[tNumber].detach();
}

void threadEntry(Synchronization* sInfo, bool* stillWorking, std::vector<long long>* messages) {
    std::unique_lock<std::mutex> lock(sInfo->mainMutex);
    auto* signal = &(sInfo->event);      /* For synchronization with main thread. */

    auto* writingMutex = &(sInfo->writingMutex); /* For shared writing. */

    while(*stillWorking) {
        signal->wait(lock); /* Waiting for the signal from main thread. */

        auto socket_ = sInfo->socketToListen;
        auto message = getMessage(socket_);

        if(message > 0){
            writingMutex->lock();
            messages->push_back(message);
            writingMutex->unlock();
        }
        if(message == -255) /* Terminating message. */
            *stillWorking = false;
    }
}
long long getMessage(int socket_) {
    auto* octet = new unsigned char;

    if(recv(socket_, reinterpret_cast<char *>(octet), sizeof(char), 0) <= 0)
        return -1;

    if((static_cast<unsigned char>((*octet)) & 0b10000000) == 0) /* Short coding. */
        return LengthWrapper(*octet).value();

    if(static_cast<unsigned char>((*octet)) == 0b11111111) /* Terminating message. */
        return -255;

    const size_t ptrLen = ((*octet) & 0b01111111) + 1;
    auto* ptr = new unsigned char[ptrLen];
    *ptr = *octet;

    for(auto i = 1; i < ptrLen; ++i)
        if(recv(socket_, reinterpret_cast<char *>(octet), sizeof(char), 0) <= 0)
            return -1;
        else
            ptr[i] = *octet;

    return LengthWrapper(ptr, ptrLen).value();
}

int Server::init() {
#ifdef _WIN32
    WSADATA wsa_data;
    return (0 == WSAStartup(MAKEWORD(2, 2), &wsa_data));
#else
    return 1;
#endif
}
void Server::deinit() {
#ifdef _WIN32
    WSACleanup();
#else
#endif
}
int setNonBlock(int socket_) {
#ifdef _WIN32
    unsigned long mode = 1;
    return ioctlsocket(socket_, FIONBIO, &mode);
#else
    int fl = fcntl(socket_, F_GETFL, 0);
    return fcntl(socket_, F_SETFL, fl | O_NONBLOCK);
#endif
}
void s_close(int s){
#ifdef _WIN32
    closesocket(s);
#else
    close(s);
#endif
}
std::string ipToString(unsigned ipNumber){
    auto* tempNum = new unsigned [4];
    *tempNum = (ipNumber >> 24) & 0xFF;
    *(tempNum + 1) = (ipNumber >> 16) & 0xFF;
    *(tempNum + 2) = (ipNumber >> 8) & 0xFF;
    *(tempNum + 3) = (ipNumber) & 0xFF;
    std::string ip = std::to_string(*tempNum) + "."
                     + std::to_string(tempNum[1]) + "."
                     + std::to_string(tempNum[2]) + "."
                     + std::to_string(tempNum[3]);
    delete [] tempNum;
    return ip;
}