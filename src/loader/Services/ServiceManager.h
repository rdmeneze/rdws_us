#pragma once

#include <list>
#include "ServiceInstance.h"

namespace loader {
  class ServiceManager {
    private:
      // list of active service instances
      std::list<loader::ServiceInstance> runningInstances;
      std::list<loader::Service> serviceConfigs;

    public:
      ServiceManager() = default;
      explicit ServiceManager(const std::list<Service>& services);

      ~ServiceManager() = default;

      bool startService(const std::string& serviceId);
      bool stopService(const std::string& serviceId);
      bool startAllServices();
      bool stopAllServices();
      [[nodiscard]] std::list<ServiceInstance> getRunningInstances() const { return runningInstances; }

  };
}