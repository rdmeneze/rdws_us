//
// Service Test Harness
// Utility to test individual services standalone via socket communication
//

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <cstring>
#include <chrono>
#include <thread>
#include <vector>

class ServiceTestHarness {
private:
    std::string serviceName;
    std::string endpoint;
    int socketFd = -1;
    
public:
    ServiceTestHarness(const std::string& name, const std::string& ep) 
        : serviceName(name), endpoint(ep) {}
    
    ~ServiceTestHarness() {
        disconnect();
    }
    
    bool connectToService() {
        std::cout << "[TEST_HARNESS] Connecting to service " << serviceName 
                  << " at " << endpoint << std::endl;
        
        socketFd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (socketFd == -1) {
            std::cerr << "[TEST_HARNESS] Failed to create socket" << std::endl;
            return false;
        }
        
        struct sockaddr_un addr;
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, endpoint.c_str(), sizeof(addr.sun_path) - 1);
        
        // Try to connect
        if (connect(socketFd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
            std::cerr << "[TEST_HARNESS] Failed to connect to service: " 
                      << strerror(errno) << std::endl;
            close(socketFd);
            socketFd = -1;
            return false;
        }
        
        std::cout << "[TEST_HARNESS] Connected successfully!" << std::endl;
        return true;
    }
    
    void disconnect() {
        if (socketFd != -1) {
            close(socketFd);
            socketFd = -1;
            std::cout << "[TEST_HARNESS] Disconnected from service" << std::endl;
        }
    }
    
    bool sendRequest(const std::string& message) {
        if (socketFd == -1) {
            std::cerr << "[TEST_HARNESS] Not connected to service" << std::endl;
            return false;
        }
        
        std::cout << "[TEST_HARNESS] Sending: " << message << std::endl;
        
        if (send(socketFd, message.c_str(), message.length(), 0) == -1) {
            std::cerr << "[TEST_HARNESS] Failed to send message" << std::endl;
            return false;
        }
        
        return true;
    }
    
    std::string receiveResponse() {
        if (socketFd == -1) {
            std::cerr << "[TEST_HARNESS] Not connected to service" << std::endl;
            return "";
        }
        
        char buffer[1024];
        ssize_t bytesRead = recv(socketFd, buffer, sizeof(buffer) - 1, 0);
        
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::string response(buffer);
            std::cout << "[TEST_HARNESS] Received: " << response << std::endl;
            return response;
        } else if (bytesRead == 0) {
            std::cout << "[TEST_HARNESS] Service closed connection" << std::endl;
        } else {
            std::cerr << "[TEST_HARNESS] Failed to receive response" << std::endl;
        }
        
        return "";
    }
    
    bool pingService() {
        return sendRequest("PING") && !receiveResponse().empty();
    }
    
    void runInteractiveMode() {
        std::cout << "\n=== Interactive Test Mode ===" << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  ping         - Send ping to service" << std::endl;
        std::cout << "  send <msg>   - Send custom message" << std::endl;
        std::cout << "  quit         - Exit interactive mode" << std::endl;
        std::cout << "================================\n" << std::endl;
        
        std::string input;
        while (true) {
            std::cout << "harness> ";
            std::getline(std::cin, input);
            
            if (input == "quit" || input == "exit") {
                break;
            } else if (input == "ping") {
                pingService();
            } else if (input.starts_with("send ")) {
                std::string message = input.substr(5);
                sendRequest(message);
                receiveResponse();
            } else if (input.empty()) {
                continue;
            } else {
                std::cout << "Unknown command: " << input << std::endl;
            }
        }
    }
};

void printUsage(const char* progName) {
    std::cout << "Usage: " << progName << " <service_name> <endpoint> [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -i, --interactive   Run in interactive mode" << std::endl;
    std::cout << "  -p, --ping          Send ping and exit" << std::endl;
    std::cout << "  -m, --message <msg> Send custom message and exit" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << progName << " example_service /tmp/example_service.sock --ping" << std::endl;
    std::cout << "  " << progName << " example_service /tmp/example_service.sock -i" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string serviceName = argv[1];
    std::string endpoint = argv[2];
    
    ServiceTestHarness harness(serviceName, endpoint);
    
    if (!harness.connectToService()) {
        std::cerr << "Failed to connect to service. Is it running?" << std::endl;
        return 1;
    }
    
    // Parse options
    bool interactive = false;
    bool ping = false;
    std::string customMessage;
    
    for (int i = 3; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-i" || arg == "--interactive") {
            interactive = true;
        } else if (arg == "-p" || arg == "--ping") {
            ping = true;
        } else if (arg == "-m" || arg == "--message") {
            if (i + 1 < argc) {
                customMessage = argv[++i];
            } else {
                std::cerr << "Error: --message requires a message argument" << std::endl;
                return 1;
            }
        }
    }
    
    // Execute based on options
    if (ping) {
        bool success = harness.pingService();
        return success ? 0 : 1;
    } else if (!customMessage.empty()) {
        harness.sendRequest(customMessage);
        harness.receiveResponse();
        return 0;
    } else if (interactive) {
        harness.runInteractiveMode();
        return 0;
    } else {
        // Default: interactive mode
        harness.runInteractiveMode();
        return 0;
    }
}