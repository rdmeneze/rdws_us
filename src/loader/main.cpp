#include <iostream>
#include <filesystem>
#include "Loader.h"
#include "Services.h"
#include "Utils.h"

int main(const int argc, char* argv[]) {
    
    // Test the services loader
    try {
        // Determine services.json path relative to executable or current directory
        std::filesystem::path servicesFile = rdws::loader::utils::resolveServiceConfigFilePath(argc, argv);
        
        
        if (std::filesystem::exists(servicesFile)) {
            std::cout << "\n--- Loading Services ---" << std::endl;
            std::cout << "Using services file: " << servicesFile << std::endl;
            
            loader::Services serviceLoader(servicesFile);
            
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
            
        } else {
            std::cout << "\nTried looking for services.json in:" << std::endl;
            std::cout << "  1. Command line argument (if provided)" << std::endl;
            std::cout << "  2. Same directory as executable" << std::endl;
            std::cout << "  3. Current working directory (./services.json)" << std::endl;
            std::cout << "  4. Project directory (src/loader/services.json)" << std::endl;
            std::cout << "\nUsage: " << argv[0] << " [path/to/services.json]" << std::endl;
            std::cout << "Services file not found: " << servicesFile << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading services: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}



