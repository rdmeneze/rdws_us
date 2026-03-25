

#include "ServiceManager.h"
#include <algorithm>
#include <iostream>
#include "Service.h"

namespace loader {
    ServiceManager::ServiceManager(const std::list<Service>& services) : serviceConfigs(services) {
        // For each service config, create the specified number of instances
        for (const auto& service : serviceConfigs) {
            if (service.isActive()) {
                for (int i = 0; i < service.getInstances(); ++i) {
                    runningInstances.emplace_back(service, i);
                }
            }
        }
    }

    bool ServiceManager::startAllServices() {
        bool allStarted = true;
        for (auto& instance : runningInstances) {
            if (!instance.start()) {
                std::cerr << "Failed to start service instance: " << instance.getServiceId() << std::endl;
                allStarted = false;
            }
        }
        return allStarted;
    }


    bool ServiceManager::stopAllServices() {
        bool allStopped = true;
        for (auto& instance : runningInstances) {
            if (!instance.stop()) {
                std::cerr << "Failed to stop service instance: " << instance.getServiceId() << std::endl;
                allStopped = false;
            }
        }
        return allStopped;
    }

    bool ServiceManager::startService(const std::string& serviceId){

        const auto instance = std::ranges::find_if(runningInstances, [&serviceId](const ServiceInstance& instance) {
          return instance.getServiceName() == serviceId && instance.getState() == ServiceInstance::State::NON_INITIALIZED;
        });

        if (instance == runningInstances.end()) {
            std::cerr << "No non-initialized instance found for service: " << serviceId << std::endl;
            return false;
        }

        return instance->start();

    }

    bool ServiceManager::stopService(const std::string& serviceId){
        const auto instance = std::ranges::find_if(runningInstances, [&serviceId](const ServiceInstance& instance) {
            return instance.getServiceName() == serviceId && instance.getState() == ServiceInstance::State::RUNNING;
        });

        if (instance == runningInstances.end()) {
            std::cerr << "No running instance found for service: " << serviceId << std::endl;
            return false;
        }

        return instance->stop();
    }
}
