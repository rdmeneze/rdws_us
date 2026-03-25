//
// Service Manager Test Application
// Demonstrates how to use ServiceManager to load and manage services
//

#include "Services/Services.h"
#include "Services/ServiceManager.h"
#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>

int main() {
    try {
        std::filesystem::path configFile = "example_services.json";
        
        std::cout << "=== Service Manager Test ===" << std::endl;
        std::cout << "Loading services from: " << configFile << std::endl;
        
        // Load service configurations
        loader::Services servicesConfig(configFile);
        
        // Create service manager
        loader::ServiceManager manager(servicesConfig.getLoadedServices());
        
        std::cout << "\n=== Starting Services ===" << std::endl;
        
        // Start example_service
        if (manager.startService("example_service")) {
            std::cout << "Successfully started example_service" << std::endl;
        } else {
            std::cout << "Failed to start example_service" << std::endl;
        }
        
        // Let it run for a bit
        std::cout << "\n=== Services Running ===" << std::endl;
        std::cout << "Letting services run for 10 seconds..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(10));
        
        std::cout << "\n=== Stopping Services ===" << std::endl;
        
        // Stop the service
        if (manager.stopService("example_service")) {
            std::cout << "Successfully stopped example_service" << std::endl;
        } else {
            std::cout << "Failed to stop example_service" << std::endl;
        }
        
        std::cout << "\n=== Test Complete ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}