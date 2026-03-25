#include <iostream>
#include <filesystem>
#include <csignal>
#include <thread>
#include <chrono>
#include <config/config.h>

#include "Loader.h"
#include "Services/ServiceManager.h"
#include "Services/Services.h"
#include "Utils/Utils.h"

// Global service manager for signal handling
static loader::ServiceManager* g_serviceManager = nullptr;

// Signal handler for graceful shutdown
void signalHandler(int signal) {
    std::cout << "\nReceived signal " << signal << ", shutting down services..." << std::endl;
    
    if (g_serviceManager != nullptr) {
        g_serviceManager->stopAllServices();
    }
    
    // Restore default signal handler and re-raise signal
    std::signal(signal, SIG_DFL);
    std::raise(signal);
}

int main(const int argc, char* argv[]) {

    rdws::Config config;
    
    // Test the services loader
    try {
        // Determine services.json path relative to executable or current directory
        const std::filesystem::path servicesFile = rdws::loader::utils::resolveServiceConfigFilePath(argc, argv);
        
        if (!std::filesystem::exists(servicesFile)) {
            std::cout << "\nTried looking for services.json in:" << std::endl;
            std::cout << "  1. Command line argument (if provided)" << std::endl;
            std::cout << "  2. Same directory as executable" << std::endl;
            std::cout << "  3. Current working directory (./services.json)" << std::endl;
            std::cout << "  4. Project directory (src/loader/services.json)" << std::endl;
            std::cout << "\nUsage: " << argv[0] << " [path/to/services.json]" << std::endl;
            std::cout << "Services file not found: " << servicesFile << std::endl;
            return 0;
        }

        std::cout << "\n--- Loading Services ---" << std::endl;
        std::cout << "Using services file: " << servicesFile << std::endl;

        const loader::Services serviceLoader(servicesFile);

        std::cout << "Loaded " << serviceLoader.getServiceCount() << " services:" << std::endl;

        for (const auto& service : serviceLoader) {
            std::cout << "  - " << service.getName()
                     << " (path: " << service.getPath().string()
                     << ", instances: " << service.getInstances() << ")" << std::endl;
        }

        // Test finding a service
        auto* foundService = serviceLoader.findServiceByName("service-test1");
        if (foundService) {
            std::cout << "\nFound service 'service-test1': " << foundService->getPath() << std::endl;
        } else {
            std::cout << "\nService 'service-test1' not found" << std::endl;
        }

        loader::ServiceManager serviceManager(serviceLoader.getLoadedServices());
        g_serviceManager = &serviceManager;  // Set global reference for signal handler

        // Register signal handlers for graceful shutdown
        std::signal(SIGINT, signalHandler);   // Ctrl+C
        std::signal(SIGTERM, signalHandler);  // kill command

        serviceManager.startAllServices();
        
        std::cout << "\nServices started. Press Ctrl+C to stop all services and exit." << std::endl;
        
        // Keep the loader running until interrupted
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

    } catch (const std::exception& e) {
        std::cerr << "Error loading services: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}



