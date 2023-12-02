#include <iostream>
#include <thread>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

void handleClient(int clientSocket) {
    // Handle client request
    char buffer[1024] = {0};
    read(clientSocket, buffer, 1024);
    std::cout << "Request: " << buffer << std::endl;

    // Send response
    const char* response = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\nHello from server!";
    write(clientSocket, response, strlen(response));

    // Close the socket
    close(clientSocket);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);

    // Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Can't create a socket!";
        return -1;
    }

    // Bind the socket to an IP / port
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    // Listen
    listen(serverSocket, 5);

    std::vector<std::thread> threads;

    while (true) {
        // Accept a call
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket == -1) {
            std::cerr << "Problem with client connecting!";
            continue;
        }

        // Handle client in a separate thread
        threads.emplace_back(handleClient, clientSocket);
    }

    // Join threads (this will not happen in this simple example, but is good practice)
    for (std::thread &t : threads) {
        if (t.joinable()) t.join();
    }

    close(serverSocket);

    return 0;
}
