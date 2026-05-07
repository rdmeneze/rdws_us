#include <iostream>
#include <csignal>
#include <thread>
#include <chrono>

#include "Services/ServiceBroker.h"

using namespace servicebroker;

// Global service broker for signal handling
static ServiceBroker *g_serviceBroker = nullptr;

// Signal handler for graceful shutdown
void signalHandler(const int signal)
{
    std::cout << "\nReceived signal " << signal << ", shutting down ServiceBroker..." << '\n';

    if (g_serviceBroker != nullptr)
    {
        g_serviceBroker->stop();
    }

    // Restore default signal handler and re-raise signal
    std::signal(signal, SIG_DFL);
    std::raise(signal);
}

int main(const int argc, char *argv[])
{
    std::cout << "=== ServiceBroker v2.0 ===\n";

    try
    {
        // Parse command line arguments
        int port = 8080;
        std::string unixSocket = "/tmp/service_broker.sock";

        if (argc >= 2)
        {
            port = std::stoi(argv[1]);
        }
        if (argc >= 3)
        {
            unixSocket = argv[2];
        }

        std::cout << "Starting ServiceBroker:" << '\n';
        std::cout << "  TCP Port: " << port << '\n';
        std::cout << "  UNIX Socket: " << unixSocket << '\n';

        // Create and start ServiceBroker
        ServiceBroker broker(port, unixSocket);
        g_serviceBroker = &broker;

        // Register signal handlers for graceful shutdown
        std::signal(SIGINT, signalHandler);  // Ctrl+C
        std::signal(SIGTERM, signalHandler); // kill command

        // Start the broker
        broker.start();

        std::cout << "\nServiceBroker started successfully!" << '\n';
        std::cout << "Services can connect to:" << '\n';
        std::cout << "  TCP: tcp://localhost:" << port << '\n';
        std::cout << "  UNIX: unix://" << unixSocket << '\n';
        std::cout << "\nPress Ctrl+C to stop." << '\n';

        // Keep running until interrupted
        while (broker.isRunning())
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error starting ServiceBroker: " << e.what() << '\n';
        return 1;
    }

    std::cout << "ServiceBroker shutdown complete." << '\n';
    return 0;
}
