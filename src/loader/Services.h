#ifndef RDWS_US_SERVICES_H
#define RDWS_US_SERVICES_H

#include <filesystem>
#include <list>
#include <string>
#include <vector>
#include "Service.h"
#include "validator/schema_validator.h"


namespace loader {
    class Services {
        public:
            explicit Services(const std::filesystem::path& configFile);
            ~Services() = default;

            // Access methods
            [[nodiscard]] const std::list<Service>& getServices() const { return loadedServices; }
            [[nodiscard]] size_t getServiceCount() const { return loadedServices.size(); }
            [[nodiscard]] bool isEmpty() const { return loadedServices.empty(); }
            
            // Find service by name
            [[nodiscard]] const Service* findServiceByName(const std::string& name) const;
            
            // Iterator support
            [[nodiscard]] std::list<Service>::const_iterator begin() const { return loadedServices.begin(); }
            [[nodiscard]] std::list<Service>::const_iterator end() const { return loadedServices.end(); }

        private:
            std::list<Service> loadedServices;
    };
} // loader

#endif //RDWS_US_SERVICES_H