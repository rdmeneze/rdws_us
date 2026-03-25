#pragma once
#include <string>
#include "Service.h"

namespace loader {
    class ServiceInstance {
      public: 
        enum class State {
          NON_INITIALIZED,
          STOPPED,
          STARTING,
          RUNNING,
          STOPPING
        };

        ServiceInstance(const loader::Service& config, int instanceNum);

        [[nodiscard]] std::string getServiceId() const { return serviceId; }
        [[nodiscard]] std::string getServiceName() const { return serviceName; }
        [[nodiscard]] std::string getBaseEndpoint() const { return baseEndpoint; }
        [[nodiscard]] State getState() const { return state; }
        [[nodiscard]] int getInstanceId() const { return instanceId; }
        [[nodiscard]] const loader::Service& getServiceConfig() const { return serviceConfig.get(); }

        [[nodiscard]] bool start();
        [[nodiscard]] bool stop();

      private: 
        bool waitForHandshake() const;
        
        std::string serviceId;
        std::string serviceName;
        std::string baseEndpoint;
        State state;
        int instanceId;
        pid_t processId;
        std::reference_wrapper<const loader::Service> serviceConfig;
    };
}