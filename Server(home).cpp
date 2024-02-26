#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

const int PORT = 8080;
const int MAX_BUFFER_SIZE = 1024;

class HomeAutomationServer {
public:
    HomeAutomationServer() {
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1) {
            perror("Error creating server socket");
            exit(EXIT_FAILURE);
        }

        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(PORT);

        if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
            perror("Error binding server socket");
            exit(EXIT_FAILURE);
        }

        if (listen(serverSocket, 5) == -1) {
            perror("Error listening on server socket");
            exit(EXIT_FAILURE);
        }

        std::cout << "Server is listening on port " << PORT << std::endl;
    }

    void handleClient(int clientSocket) {
        char buffer[MAX_BUFFER_SIZE] = {0};
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesRead == -1) {
            perror("Error reading from client");
            close(clientSocket);
            return;
        }

        std::string request(buffer);

        std::string device, action;
        size_t commaPos = request.find(',');
        if (commaPos != std::string::npos) {
            device = request.substr(0, commaPos);
            action = request.substr(commaPos + 1);
        }

        std::string response;
        if (isValidCommand(device, action)) {
            devices[device] = action;
            std::cout << device << " is turned " << action << std::endl;
            response = device + " is now " + action;
        } else {
            response = "Invalid command: " + request;
        }

        send(clientSocket, response.c_str(), response.length(), 0);
        close(clientSocket);
    }

    void start() {
        while (true) {
            int clientSocket = accept(serverSocket, nullptr, nullptr);
            if (clientSocket == -1) {
                perror("Error accepting connection");
                continue;
            }

            std::cout << "Accepted connection from client" << std::endl;

            std::thread(&HomeAutomationServer::handleClient, this, clientSocket).detach();
        }
    }

private:
    int serverSocket;
    sockaddr_in serverAddress;
    std::map<std::string, std::string> devices{{"lamp", "OFF"}, {"fan", "OFF"}, {"air_cooler", "OFF"}, {"washing_machine", "OFF"}};

    bool isValidCommand(const std::string& device, const std::string& action) const {
        return devices.count(device) && (action == "ON" || action == "OFF");
    }
};

int main() {
    HomeAutomationServer server;
    server.start();

    return 0;
}
