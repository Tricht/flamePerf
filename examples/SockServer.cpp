#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <thread>
#include <unistd.h>

void start_tcp_server(int port)
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Bind the socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    std::cout << "TCP server listening on port " << port << std::endl;
    while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) >= 0)
    {
        std::cout << "Client connected." << std::endl;

        // Loop to keep connection open and read multiple messages
        while (true)
        {
            memset(buffer, 0, 1024); // Clear buffer
            int readBytes = read(new_socket, buffer, 1024);
            if (readBytes <= 0)
            {
                // If read returns 0, the client has closed the connection
                std::cout << "Client disconnected." << std::endl;
                break;
            }
            std::cout << "Received TCP data: " << buffer << std::endl;
        }
        close(new_socket);
    }
    close(server_fd);
}

void start_udp_server(int port)
{
    int sockfd;
    char buffer[1024];
    struct sockaddr_in servaddr, cliaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "UDP server listening on port " << port << std::endl;
    int len = sizeof(cliaddr); // len is value/result

    while (true)
    {
        int n = recvfrom(sockfd, (char *)buffer, 1024, MSG_WAITALL, (struct sockaddr *)&cliaddr, (socklen_t *)&len);
        buffer[n] = '\0';
        std::cout << "Received UDP data: " << buffer << std::endl;
    }

    close(sockfd);
}

int main()
{
    int tcp_port = 8042;
    int udp_port = 8043;

    // Starting TCP server on a separate thread
    std::thread tcp_thread([=]()
                           { start_tcp_server(tcp_port); });

    // Starting UDP server on a separate thread
    std::thread udp_thread([=]()
                           { start_udp_server(udp_port); });

    tcp_thread.join();
    udp_thread.join();

    return 0;
}
