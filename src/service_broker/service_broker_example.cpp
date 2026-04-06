#include "../Services/ServiceBroker.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace servicebroker;

int main() {
    std::cout << "=== ServiceBroker Example ===" << std::endl;
    
    // Create and start broker
    ServiceBroker broker(8080, "/tmp/example_service_broker.sock");
    
    if (!broker.start()) {
        std::cerr << "Failed to start broker" << std::endl;
        return 1;
    }
    
    std::cout << "\nBroker started! Waiting for service connections..." << std::endl;
    std::cout << "Services can connect via:" << std::endl;
    std::cout << "  TCP: localhost:8080" << std::endl;
    std::cout << "  UNIX: /tmp/example_service_broker.sock" << std::endl;
    
    // Monitor broker status
    std::thread monitorThread([&broker]() {
        while (broker.isRunning()) {
            std::this_thread::sleep_for(std::chrono::seconds(10));
            
            auto status = broker.getBrokerStatus();
            std::cout << "\n--- Broker Status ---" << std::endl;
            std::cout << "Active Connections: " << status["activeConnections"].asInt() << std::endl;
            std::cout << "Registered Services: " << status["registryStatus"]["totalServices"].asInt() << std::endl;
            std::cout << "Healthy Services: " << status["registryStatus"]["healthyServices"].asInt() << std::endl;
            
            // Show connection details
            auto connections = broker.getConnectionStatus();
            if (connections.isArray() && connections.size() > 0) {
                std::cout << "\nActive Connections:" << std::endl;
                for (const auto& conn : connections) {
                    std::cout << "  fd=" << conn["fd"].asInt() 
                              << " type=" << conn["type"].asString()
                              << " addr=" << conn["address"].asString();
                    if (conn["identified"].asBool()) {
                        std::cout << " serviceId=" << conn["serviceId"].asString();
                    } else {
                        std::cout << " (not identified)";
                    }
                    std::cout << " uptime=" << conn["uptimeSeconds"].asInt64() << "s" << std::endl;
                }
            }
            
            // Show registered services
            const auto& registry = broker.getRegistry();
            auto serviceIds = registry.getAllServiceIds();
            if (!serviceIds.empty()) {
                std::cout << "\nRegistered Services:" << std::endl;
                for (const auto& serviceId : serviceIds) {
                    const auto* identity = registry.findServiceById(serviceId);
                    if (identity) {
                        std::cout << "  " << serviceId << " (" << identity->serviceName 
                                  << ") on " << identity->machineName 
                                  << " - Load: " << identity->getLoadPercentage() << "%"
                                  << " - Healthy: " << (identity->isHealthy() ? "Yes" : "No") << std::endl;
                        
                        // Show capabilities
                        if (!identity->capabilities.empty()) {
                            std::cout << "    Capabilities: ";
                            for (size_t i = 0; i < identity->capabilities.size(); ++i) {
                                std::cout << identity->capabilities[i];
                                if (i < identity->capabilities.size() - 1) std::cout << ", ";
                            }
                            std::cout << std::endl;
                        }
                    }
                }
            }
        }
    });
    
    std::cout << "\nPress Enter to stop broker..." << std::endl;
    std::cin.get();
    
    broker.stop();
    monitorThread.join();
    
    std::cout << "Broker stopped." << std::endl;
    return 0;
}