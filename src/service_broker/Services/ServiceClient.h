#pragma once

#include "ServiceIdentity.h"
#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <json/json.h>

namespace servicebroker {

using RequestHandler = std::function<Json::Value(const Json::Value& request)>;

class ServiceClient {
private:
    ServiceIdentity identity;
    std::string brokerAddress;  // "tcp://localhost:8080" or "unix:///tmp/service_broker.sock"
    int socketFd = -1;
    
    std::atomic<bool> connected{false};
    std::atomic<bool> registered{false};
    std::thread messageThread;
    std::thread pingThread;
    
    RequestHandler requestHandler;
    
public:
    explicit ServiceClient(const ServiceIdentity& serviceIdentity, 
                          const std::string& address = "unix:///tmp/service_broker.sock");
    ~ServiceClient();
    
    // Connection management
    bool connect();
    void disconnect();
    bool isConnected() const { return connected.load(); }
    bool isRegistered() const { return registered.load(); }
    
    // Service registration
    bool registerService();
    
    // Request handling
    void setRequestHandler(RequestHandler handler);
    
    // Communication
    bool sendPing(const Json::Value& stats = Json::Value());
    bool sendResponse(const std::string& requestId, const Json::Value& response);
    
    // Main event loop
    void run();
    void stop();
    
private:
    // Socket management
    int createConnection();
    bool sendMessage(const Json::Value& message);
    void messageLoop();
    void pingLoop();
    
    // Message handlers
    void handleMessage(const std::string& message);
    void handleRequest(const Json::Value& message);
    void handlePong(const Json::Value& message);
};

} // namespace servicebroker