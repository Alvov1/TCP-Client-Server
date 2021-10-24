#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <winsock2.h>
    #include <WS2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else // LINUX
    #include <sys/poll.h>
    #include <fcntl.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <cerrno>
    #include <unistd.h>
#endif

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include "../LengthWrapper.h"

#define CLIENTS_LIMIT 20
const auto threadsNumber = 2;
const auto messagesLimit = 100;

struct Synchronization {
    std::mutex mainMutex;
    std::condition_variable event;          /* For synchronization with main thread. */

    std::mutex writingMutex;                /* For shared writing. */

    int socketToListen = -1;
};

class Server {
    int socket_;
    struct sockaddr_in address{};
    unsigned socketCount = 0;
    int cs[CLIENTS_LIMIT]{};
    struct pollfd pfd[CLIENTS_LIMIT + 1]{};
    unsigned port;

    std::vector<std::thread> threads;
    Synchronization syncInfo;

    std::vector<long long> messages;

    bool working = true;

    int init();
    void deinit();
    void createThreads();

public:
    Server(unsigned Port);
    ~Server();
    Server& operator=(const Server& copy) = delete;

    void listenClients();
};

long long getMessage(int socket_);
void threadEntry(Synchronization* sInfo, bool* stillWorking, std::vector<long long>* messages);
int setNonBlock(int socket_);
void s_close(int s);
std::string ipToString(unsigned ipNumber);
#endif //SERVER_SERVER_H