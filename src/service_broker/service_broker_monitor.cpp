#include "../Services/ServiceBroker.h"
#include "../Services/ServiceMonitor.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <signal.h>

using namespace servicebroker;

// Global variables for signal handling
ServiceBroker* g_broker = nullptr;
bool g_running = true;

void signalHandler(int signal) {
    std::cout << "\nReceived signal " << signal << ", shutting down..." << std::endl;
    g_running = false;
    if (g_broker) {
        g_broker->stop();
    }
}

int main() {
    // Set up signal handling
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    std::cout << "=== Service Broker with Monitoring Interface ===" << std::endl;
    
    // Create and start broker
    ServiceBroker broker(8080, "/tmp/example_service_broker.sock");
    g_broker = &broker;
    
    if (!broker.start()) {
        std::cerr << "Failed to start broker" << std::endl;
        return 1;
    }
    
    // Create monitor
    ServiceMonitor monitor(broker);
    
    std::cout << "\nBroker started with monitoring interface!" << std::endl;
    std::cout << "Services can connect via:" << std::endl;
    std::cout << "  TCP: localhost:8080" << std::endl;
    std::cout << "  UNIX: /tmp/example_service_broker.sock" << std::endl;
    std::cout << "\nMonitoring interface commands:" << std::endl;
    std::cout << "  s - Show current status" << std::endl;
    std::cout << "  c - Start continuous monitoring" << std::endl;
    std::cout << "  h - Show health status" << std::endl;
    std::cout << "  f - Save status to file" << std::endl;
    std::cout << "  q - Quit" << std::endl;
    std::cout << "  help - Show this help" << std::endl;
    
    // Interactive command loop
    std::string command;
    while (g_running && broker.isRunning()) {
        std::cout << "\nbroker> ";
        std::getline(std::cin, command);
        
        if (command == "q" || command == "quit") {
            g_running = false;
            break;
        } else if (command == "s" || command == "status") {
            monitor.displayStatus();
        } else if (command == "c" || command == "continuous") {
            std::cout << "Starting continuous monitoring (Press Ctrl+C to stop)..." << std::endl;
            std::thread monitorThread([&monitor]() {
                monitor.displayContinuous(5);
            });
            
            // Wait for interrupt
            while (g_running && broker.isRunning()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            
            monitorThread.detach(); // Will stop when broker stops
        } else if (command == "h" || command == "health") {
            monitor.showHealthStatus();
        } else if (command == "f" || command == "file") {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            std::ostringstream filename;
            filename << "broker_status_" << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S") << ".txt";
            
            monitor.saveStatusToFile(filename.str());
        } else if (command.starts_with("cap ")) {
            std::string capability = command.substr(4);
            monitor.showServicesByCapability(capability);
        } else if (command.starts_with("machine ")) {
            std::string machine = command.substr(8);
            monitor.showServicesByMachine(machine);
        } else if (command == "help" || command == "?") {
            std::cout << "\nCommands:" << std::endl;
            std::cout << "  s, status      - Show current broker/service status" << std::endl;
            std::cout << "  c, continuous  - Start continuous monitoring display" << std::endl;
            std::cout << "  h, health      - Show service health status" << std::endl;
            std::cout << "  f, file        - Save status to timestamped file" << std::endl;
            std::cout << "  cap <name>     - Show services with capability" << std::endl;
            std::cout << "  machine <name> - Show services on machine" << std::endl;
            std::cout << "  q, quit        - Quit broker" << std::endl;
            std::cout << "  help, ?        - Show this help" << std::endl;
        } else if (!command.empty()) {
            std::cout << "Unknown command: " << command << std::endl;
            std::cout << "Type 'help' for available commands" << std::endl;
        }
    }
    
    std::cout << "\nStopping broker..." << std::endl;
    broker.stop();
    
    std::cout << "Broker stopped." << std::endl;
    return 0;
}