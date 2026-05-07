#include "../Services/ServiceBroker.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace servicebroker;

int main()
{
    std::cout << "=== ServiceBroker Example ===" << '\n';

    // Create and start broker
    ServiceBroker broker(8080, "/tmp/example_service_broker.sock");

    if (!broker.start())
    {
        std::cerr << "Failed to start broker" << '\n';
        return 1;
    }

    std::cout << "\nBroker started! Waiting for service connections..." << '\n';
    std::cout << "Services can connect via:" << '\n';
    std::cout << "  TCP: localhost:8080" << '\n';
    std::cout << "  UNIX: /tmp/example_service_broker.sock" << '\n';

    // Monitor broker status
    std::thread monitorThread([&broker]()
                              {
        while (broker.isRunning()) {
            std::this_thread::sleep_for(std::chrono::seconds(10));
            
            auto status = broker.getBrokerStatus();
            std::cout << "\n--- Broker Status ---" << '\n';
            std::cout << "Active Connections: " << status["activeConnections"].asInt() << '\n';
            std::cout << "Registered Services: " << status["registryStatus"]["totalServices"].asInt() << '\n';
            std::cout << "Healthy Services: " << status["registryStatus"]["healthyServices"].asInt() << '\n';
            
            // Show connection details
            auto connections = broker.getConnectionStatus();
            if (connections.isArray() && connections.size() > 0) {
                std::cout << "\nActive Connections:" << '\n';
                for (const auto& conn : connections) {
                    std::cout << "  fd=" << conn["fd"].asInt() 
                              << " type=" << conn["type"].asString()
                              << " addr=" << conn["address"].asString();
                    if (conn["identified"].asBool()) {
                        std::cout << " serviceId=" << conn["serviceId"].asString();
                    } else {
                        std::cout << " (not identified)";
                    }
                    std::cout << " uptime=" << conn["uptimeSeconds"].asInt64() << "s" << '\n';
                }
            }
            
            // Show registered services
            const auto& registry = broker.getRegistry();
            auto serviceIds = registry.getAllServiceIds();
            if (!serviceIds.empty()) {
                std::cout << "\nRegistered Services:" << '\n';
                for (const auto& serviceId : serviceIds) {
                    const auto* identity = registry.findServiceById(serviceId);
                    if (identity) {
                        std::cout << "  " << serviceId << " (" << identity->serviceName 
                                  << ") on " << identity->machineName 
                                  << " - Load: " << identity->getLoadPercentage() << "%"
                                  << " - Healthy: " << (identity->isHealthy() ? "Yes" : "No") << '\n';
                        
                        // Show capabilities
                        if (!identity->capabilities.empty()) {
                            std::cout << "    Capabilities: ";
                            for (size_t i = 0; i < identity->capabilities.size(); ++i) {
                                std::cout << identity->capabilities[i];
                                if (i < identity->capabilities.size() - 1) std::cout << ", ";
                            }
                            std::cout << '\n';
                        }
                    }
                }
            }
        } });

    std::cout << "\nPress Enter to stop broker..." << '\n';
    std::cin.get();

    broker.stop();
    monitorThread.join();

    std::cout << "Broker stopped." << '\n';
    return 0;
}