#include "../Services/ServiceClient.h"
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <random>

using namespace servicebroker;

// Example service that provides greeting capabilities
Json::Value processGreeting(const Json::Value& request) {
    std::string name = request.get("name", "World").asString();
    std::string language = request.get("language", "en").asString();
    
    Json::Value response;
    
    if (language == "pt") {
        response["greeting"] = "Olá, " + name + "!";
    } else if (language == "es") {
        response["greeting"] = "¡Hola, " + name + "!";
    } else {
        response["greeting"] = "Hello, " + name + "!";
    }
    
    response["service"] = "example_greeting_service";
    response["processedAt"] = static_cast<Json::Int64>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
    
    // Simulate processing time
    std::this_thread::sleep_for(std::chrono::milliseconds(100 + (rand() % 200)));
    
    return response;
}

int main(int argc, char* argv[]) {
    std::cout << "=== Example Service Client ===" << std::endl;
    
    // Parse command line arguments
    std::string brokerAddress = "unix:///tmp/example_service_broker.sock";
    std::string machineName = "localhost";
    std::string serviceId = "greeting_001";
    
    if (argc > 1) {
        brokerAddress = argv[1];
    }
    if (argc > 2) {
        machineName = argv[2];
    }
    if (argc > 3) {
        serviceId = argv[3];
    }
    
    // Create service identity
    ServiceIdentity identity;
    identity.machineName = machineName;
    identity.serviceName = "greeting_service";
    identity.serviceId = serviceId;
    identity.version = "v1.0.0";
    identity.environment = "dev";
    identity.maxConcurrent = 5;
    identity.capabilities = {"greeting", "translation", "multilingual"};
    
    std::cout << "Service Identity:" << std::endl;
    std::cout << "  Machine: " << identity.machineName << std::endl;
    std::cout << "  Service: " << identity.serviceName << std::endl;
    std::cout << "  ID: " << identity.serviceId << std::endl;
    std::cout << "  Version: " << identity.version << std::endl;
    std::cout << "  Capabilities: ";
    for (size_t i = 0; i < identity.capabilities.size(); ++i) {
        std::cout << identity.capabilities[i];
        if (i < identity.capabilities.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
    std::cout << "  Broker: " << brokerAddress << std::endl;
    
    // Create service client
    ServiceClient client(identity, brokerAddress);
    
    // Set request handler
    client.setRequestHandler(processGreeting);
    
    std::cout << "\nStarting service client..." << std::endl;
    
    // Start client in a separate thread
    std::thread clientThread([&client]() {
        client.run();
    });
    
    std::cout << "Service client started. Press Enter to stop..." << std::endl;
    std::cin.get();
    
    std::cout << "Stopping service..." << std::endl;
    client.stop();
    
    clientThread.join();
    
    std::cout << "Service stopped." << std::endl;
    return 0;
}