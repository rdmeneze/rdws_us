#include "ServiceInstance.h"
#include "ServiceIPCConfig.h"
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>
#include <spawn.h>
#include <vector>

loader::ServiceInstance::ServiceInstance(const loader::Service& config, const int instanceNum) : processId(-1), serviceConfig(config) {
    serviceId = config.getName() + "_" + std::to_string(instanceNum);
    serviceName = config.getName();
    instanceId = instanceNum;
    state = State::NON_INITIALIZED;

    // For simplicity, we assume all services use the same IPC type and base endpoint format
    // In a real implementation, this would likely come from the Service configuration
    const ServiceIPCConfig ipcConfig(IPCType(loader::IPCType::UNIX_SOCKET), "/tmp/" + config.getName() + ".sock");
    baseEndpoint = ipcConfig.generateInstanceEndpoint(instanceId);
}


bool loader::ServiceInstance::start() {
    if (state != State::NON_INITIALIZED && state != State::STOPPED) {
        std::cerr << "Cannot start service " << serviceId << " in state " << static_cast<int>(state) << std::endl;
        return false;
    }

    state = State::STARTING;
    std::cout << "Starting service: " << serviceId << " at " << baseEndpoint << std::endl;

    // Prepare arguments for the service executable
    const std::string servicePath = serviceConfig.get().getPath();
    std::cout << "Service path: " << servicePath << std::endl;  // Debug output
    
    std::vector<std::string> args = {
        servicePath,
        serviceId,
        std::to_string(instanceId),
        baseEndpoint
    };
    
    // Convert to char* array for posix_spawn
    std::vector<char*> argv;
    for (auto& arg : args) {
        argv.push_back(arg.data());
    }
    argv.push_back(nullptr);

    int result = posix_spawn(&processId, servicePath.c_str(), nullptr, nullptr, argv.data(), ::environ);
    
    if (result != 0) {
        std::cerr << "Failed to spawn process for service: " << serviceId 
                  << " (error: " << result << " - " << strerror(result) << ")" 
                  << std::endl;
        std::cerr << "Attempted to execute: " << servicePath << std::endl;
        state = State::STOPPED;
        return false;
    }
    
    std::cout << "Service process started with PID: " << processId << std::endl;
    
    // Wait for handshake
    if (!waitForHandshake()) {
        std::cerr << "Handshake failed for service: " << serviceId << std::endl;
        (void)stop();
        return false;
    }
    
    state = State::RUNNING;
    std::cout << "Service " << serviceId << " started successfully (PID: " << processId << ")" << std::endl;
    return true;
}

bool loader::ServiceInstance::stop() {
    if (state != State::RUNNING && state != State::STARTING) {
        std::cerr << "Cannot stop service " << serviceId << " in state " << static_cast<int>(state) << std::endl;
        return false;
    }
    
    state = State::STOPPING;
    std::cout << "Stopping service: " << serviceId << " (PID: " << processId << ")" << std::endl;
    
    if (processId > 0) {
        // Send SIGTERM to gracefully shutdown
        if (kill(processId, SIGTERM) == -1) {
            std::cerr << "Failed to send SIGTERM to process " << processId << std::endl;
        }
        
        // Wait for process to exit (with timeout)
        int status;
        int waitResult = waitpid(processId, &status, WNOHANG);
        
        if (waitResult == 0) {
            // Process still running after SIGTERM, wait a bit more
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            waitResult = waitpid(processId, &status, WNOHANG);
            
            if (waitResult == 0) {
                // Force kill if still running
                std::cout << "Process " << processId << " didn't respond to SIGTERM, sending SIGKILL" << std::endl;
                kill(processId, SIGKILL);
                waitpid(processId, &status, 0);
            }
        }
        
        processId = -1;
    }
    
    // Clean up socket file if it exists
    unlink(baseEndpoint.c_str());
    
    state = State::STOPPED;
    std::cout << "Service " << serviceId << " stopped" << std::endl;
    return true;
}

bool loader::ServiceInstance::waitForHandshake() const {
    // Create a UNIX socket to listen for handshake
    const int socketFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socketFd == -1) {
        std::cerr << "Failed to create handshake socket" << std::endl;
        return false;
    }
    
    struct sockaddr_un addr = {};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, baseEndpoint.c_str(), sizeof(addr.sun_path) - 1);
    
    // Remove any existing socket file
    unlink(baseEndpoint.c_str());
    
    if (bind(socketFd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == -1) {
        std::cerr << "Failed to bind handshake socket to " << baseEndpoint << std::endl;
        close(socketFd);
        return false;
    }
    
    if (listen(socketFd, 1) == -1) {
        std::cerr << "Failed to listen on handshake socket" << std::endl;
        close(socketFd);
        return false;
    }
    
    // Wait for connection with timeout (5 seconds)
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    setsockopt(socketFd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    
    int clientFd = accept(socketFd, nullptr, nullptr);
    if (clientFd == -1) {
        std::cerr << "Failed to accept handshake connection" << std::endl;
        close(socketFd);
        return false;
    }
    
    // Read handshake message
    char buffer[256];
    ssize_t bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        std::string message(buffer);
        
        if (message == "READY:" + serviceId) {
            // Send ACK
            const std::string ackMessage = "ACK:" + serviceId;
            send(clientFd, ackMessage.c_str(), ackMessage.length(), 0);
            
            close(clientFd);
            close(socketFd);
            return true;
        } else {
            std::cerr << "Invalid handshake message: " << message << std::endl;
        }
    } else {
        std::cerr << "Failed to read handshake message" << std::endl;
    }
    
    close(clientFd);
    close(socketFd);
    return false;
}
