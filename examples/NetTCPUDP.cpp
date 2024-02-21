#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

void sendTCPData(const char* serverIP, int serverPort, int numMessages) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverIP, &serverAddr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "TCP connection failed" << std::endl;
        return;
    }

    for(int i = 0; i < numMessages; ++i) {
        const char* message = "TCP message";
        send(sock, message, strlen(message), 0);
        // usleep(100000);
    }

    close(sock);
}

void sendUDPData(const char* serverIP, int serverPort, int numMessages) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverIP, &serverAddr.sin_addr);

    for(int i = 0; i < numMessages; ++i) {
        const char* message = "UDP message";
        sendto(sock, message, strlen(message), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        usleep(100000);
    }

    close(sock);
}

int main() {
    const char* serverIP = "127.0.0.1";
    int serverPort = 8080;
    int numTCP = 1;
    int numUDP = 10;

    sendTCPData(serverIP, serverPort, numTCP);
    sendUDPData(serverIP, serverPort, numUDP);

    return 0;
}

