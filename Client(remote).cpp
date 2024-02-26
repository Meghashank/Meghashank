#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

const int PORT = 8080;
const int MAX_BUFFER_SIZE = 1024;

class HomeAutomationClient {
public:
    HomeAutomationClient() {
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == -1) {
            perror("Error creating client socket");
            exit(EXIT_FAILURE);
        }

        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(PORT);

        if (inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) == -1) {
            perror("Error converting address");
            exit(EXIT_FAILURE);
        }

        if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
            perror("Error connecting to server");
            exit(EXIT_FAILURE);
        }
    }

    void sendCommand(const std::string& command) {
        send(clientSocket, command.c_str(), command.length(), 0);

        char buffer[MAX_BUFFER_SIZE] = {0};
        recv(clientSocket, buffer, sizeof(buffer), 0);

        std::cout << "Server response: " << buffer << std::endl;
    }

    ~HomeAutomationClient() {
        close(clientSocket);
    }

private:
    int clientSocket;
    sockaddr_in serverAddress;
};

int main() {
    HomeAutomationClient client;

    while (true) {
        std::cout << "Available devices: lamp, fan, air_cooler, washing_machine" << std::endl;
        std::string device, action;

        std::cout << "Enter the device you want to control: ";
        std::cin >> device;

        std::cout << "Enter the action (ON/OFF): ";
        std::cin >> action;

        std::string command = device + "," + action;
        client.sendCommand(command);
    }

    return 0;
}
