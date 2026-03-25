//
// Example Service Application
// This demonstrates how a service should interact with the ServiceLoader
//

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <thread>
#include <chrono>
#include <cstring>

class ExampleService {
private:
    std::string serviceId;
    int instanceId;
    std::string endpoint;
    bool running = false;
    bool developmentMode = false;
    int serverSocket = -1;
    
public:
    ExampleService(const std::string& id, int instance, const std::string& ep, bool devMode = false) 
        : serviceId(id), instanceId(instance), endpoint(ep), developmentMode(devMode) {}
    
    bool performHandshake() {
        std::cout << "[" << serviceId << "] Attempting handshake with loader at " << endpoint << std::endl;
        
        int socketFd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (socketFd == -1) {
            std::cerr << "[" << serviceId << "] Failed to create socket for handshake" << std::endl;
            return false;
        }
        
        struct sockaddr_un addr;
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, endpoint.c_str(), sizeof(addr.sun_path) - 1);
        
        // Retry connection a few times (loader might not be ready immediately)
        for (int i = 0; i < 5; ++i) {
            if (connect(socketFd, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            if (i == 4) {
                std::cerr << "[" << serviceId << "] Failed to connect to loader" << std::endl;
                close(socketFd);
                return false;
            }
        }
        
        // Send READY message
        std::string readyMessage = "READY:" + serviceId;
        if (send(socketFd, readyMessage.c_str(), readyMessage.length(), 0) == -1) {
            std::cerr << "[" << serviceId << "] Failed to send READY message" << std::endl;
            close(socketFd);
            return false;
        }
        
        // Wait for ACK
        char buffer[256];
        ssize_t bytesRead = recv(socketFd, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::string response(buffer);
            
            if (response == "ACK:" + serviceId) {
                std::cout << "[" << serviceId << "] Handshake successful!" << std::endl;
                close(socketFd);
                return true;
            } else {
                std::cerr << "[" << serviceId << "] Invalid ACK message: " << response << std::endl;
            }
        } else {
            std::cerr << "[" << serviceId << "] Failed to read ACK message" << std::endl;
        }
        
        close(socketFd);
        return false;
    }
    
    void run() {
        running = true;
        
        if (developmentMode) {
            std::cout << "[" << serviceId << "] Running in DEVELOPMENT mode (instance " << instanceId << ")" << std::endl;
            runDevelopmentMode();
        } else {
            std::cout << "[" << serviceId << "] Service running (instance " << instanceId << ")" << std::endl;
            runProductionMode();
        }
        
        std::cout << "[" << serviceId << "] Service stopped" << std::endl;
    }
    
private:
    void runProductionMode() {
        int counter = 0;
        while (running) {
            std::cout << "[" << serviceId << "] Working... counter=" << counter++ << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }
    
    void runDevelopmentMode() {
        if (!setupDevelopmentSocket()) {
            return;
        }
        
        std::cout << "[" << serviceId << "] Ready to accept connections at " << endpoint << std::endl;
        std::cout << "[" << serviceId << "] 🐛 DEBUG MODE: Use service_test_harness to connect!" << std::endl;
        
        while (running) {
            acceptConnection();
        }
        
        cleanupDevelopmentSocket();
    }
    
    bool setupDevelopmentSocket() {
        // Remove existing socket file if it exists
        unlink(endpoint.c_str());
        
        serverSocket = socket(AF_UNIX, SOCK_STREAM, 0);
        if (serverSocket == -1) {
            std::cerr << "[" << serviceId << "] Failed to create server socket" << std::endl;
            return false;
        }
        
        struct sockaddr_un addr;
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, endpoint.c_str(), sizeof(addr.sun_path) - 1);
        
        if (bind(serverSocket, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
            std::cerr << "[" << serviceId << "] Failed to bind socket: " << strerror(errno) << std::endl;
            close(serverSocket);
            return false;
        }
        
        if (listen(serverSocket, 5) == -1) {
            std::cerr << "[" << serviceId << "] Failed to listen on socket" << std::endl;
            close(serverSocket);
            return false;
        }
        
        return true;
    }
    
    void acceptConnection() {
        struct sockaddr_un clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket == -1) {
            if (running) { // Only log if we're still supposed to be running
                std::cerr << "[" << serviceId << "] Failed to accept connection" << std::endl;
            }
            return;
        }
        
        std::cout << "[" << serviceId << "] Client connected" << std::endl;
        
        // Handle client in separate thread to allow multiple connections
        std::thread clientThread(&ExampleService::handleClient, this, clientSocket);
        clientThread.detach();
    }
    
    void handleClient(int clientSocket) {
        char buffer[1024];
        
        while (running) {
            ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            
            if (bytesRead <= 0) {
                break; // Client disconnected or error
            }
            
            buffer[bytesRead] = '\0';
            std::string request(buffer);
            
            std::cout << "[" << serviceId << "] Received: " << request << std::endl;
            
            // Process the request and generate response
            std::string response = processRequest(request);
            
            if (send(clientSocket, response.c_str(), response.length(), 0) == -1) {
                std::cerr << "[" << serviceId << "] Failed to send response" << std::endl;
                break;
            }
            
            std::cout << "[" << serviceId << "] Sent: " << response << std::endl;
        }
        
        close(clientSocket);
        std::cout << "[" << serviceId << "] Client disconnected" << std::endl;
    }
    
    std::string processRequest(const std::string& request) {
        // TODO: Implement actual service logic here
        
        if (request == "PING") {
            return "PONG";
        } else if (request == "STATUS") {
            return "RUNNING";
        } else if (request == "INFO") {
            return "Service:" + serviceId + ",Instance:" + std::to_string(instanceId);
        } else if (request.starts_with("ECHO ")) {
            return "ECHO_RESPONSE:" + request.substr(5);
        } else {
            return "UNKNOWN_COMMAND:" + request;
        }
    }
    
    void cleanupDevelopmentSocket() {
        if (serverSocket != -1) {
            close(serverSocket);
            serverSocket = -1;
        }
        unlink(endpoint.c_str());
    }
    
public:
    
    void shutdown() {
        std::cout << "[" << serviceId << "] Received shutdown signal" << std::endl;
        running = false;
        
        if (developmentMode && serverSocket != -1) {
            // Close server socket to unblock accept()
            close(serverSocket);
            serverSocket = -1;
        }
    }
};

// Global service instance for signal handling
ExampleService* globalService = nullptr;

void signalHandler(int signal) {
    if (globalService && signal == SIGTERM) {
        globalService->shutdown();
    }
}

int main(int argc, char* argv[]) {
    bool developmentMode = false;
    
    // Check for development mode flag
    if (argc == 2 && std::string(argv[1]) == "--dev") {
        developmentMode = true;
        std::cout << "=== DEVELOPMENT MODE ===" << std::endl;
        std::cout << "Service will run standalone for debugging" << std::endl;
        std::cout << "========================" << std::endl;
    }
    
    if (!developmentMode && argc != 4) {
        std::cerr << "Usage: " << std::endl;
        std::cerr << "  Production:  " << argv[0] << " <service_id> <instance_id> <endpoint>" << std::endl;
        std::cerr << "  Development: " << argv[0] << " --dev" << std::endl;
        return 1;
    }
    
    std::string serviceId;
    int instanceId;
    std::string endpoint;
    
    if (developmentMode) {
        serviceId = "example_service_dev";
        instanceId = 0;
        endpoint = "/tmp/example_service_dev.sock";
        std::cout << "Development config:" << std::endl;
        std::cout << "  Service ID: " << serviceId << std::endl;
        std::cout << "  Endpoint: " << endpoint << std::endl;
    } else {
        serviceId = argv[1];
        instanceId = std::stoi(argv[2]);
        endpoint = argv[3];
    }
    
    std::cout << "[" << serviceId << "] Starting service with instance " << instanceId 
              << " at " << endpoint << std::endl;
    
    ExampleService service(serviceId, instanceId, endpoint, developmentMode);
    globalService = &service;
    
    // Setup signal handler for graceful shutdown
    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);  // Also handle Ctrl+C in dev mode
    
    if (!developmentMode) {
        // Perform handshake with loader (production mode only)
        if (!service.performHandshake()) {
            std::cerr << "[" << serviceId << "] Handshake failed, exiting" << std::endl;
            return 1;
        }
    }
    
    // Run the service
    service.run();
    
    return 0;
}