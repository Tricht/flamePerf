#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>

void sendTCPData(const char *serverIP, int serverPort, int numMessages)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverIP, &serverAddr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::cerr << "TCP connection failed" << std::endl;
        return;
    }

    for (int i = 0; i < numMessages; ++i)
    {
        const char *message = "TCP message";
        send(sock, message, strlen(message), 0);
        usleep(100000); // Wait for 100 milliseconds
    }

    close(sock);
}

void sendUDPData(const char *serverIP, int serverPort, int numMessages)
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverIP, &serverAddr.sin_addr);

    for (int i = 0; i < numMessages; ++i)
    {
        const char *message = "UDP message";
        sendto(sock, message, strlen(message), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
        usleep(100000); // Wait for 100 milliseconds
    }

    close(sock);
}

int main()
{
    const char *serverIP = "127.0.0.1";
    int serverTCP = 8042;
    int serverUDP = 8043;
    int numTCP = 100;
    int numUDP = 100;

    // Start TCP and UDP send in separate threads
    std::thread tcpThread(sendTCPData, serverIP, serverTCP, numTCP);
    std::thread udpThread(sendUDPData, serverIP, serverUDP, numUDP);

    // Wait for both threads to complete
    tcpThread.join();
    udpThread.join();

    return 0;
}
