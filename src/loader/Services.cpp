//
// Created by rdias on 3/12/26.
//

#include "Services.h"
#include "schemas/service.h"
#include <fstream>
#include <iostream>
#include <json/json.h>
#include <stdexcept>
#include <algorithm>

namespace loader {

    Services::Services(const std::filesystem::path& configFile) {
        // Check if file exists
        if (!std::filesystem::exists(configFile)) {
            throw std::runtime_error("Configuration file not found: " + configFile.string());
        }

        // Read JSON file
        std::ifstream file(configFile);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open configuration file: " + configFile.string());
        }

        Json::Value servicesJson;
        Json::CharReaderBuilder readerBuilder;
        Json::String errs;
        
        if (!Json::parseFromStream(readerBuilder, file, &servicesJson, &errs)) {
            throw std::runtime_error("Invalid JSON in configuration file: " + errs);
        }

        // Validate JSON against schema
        auto validator = rdws::validation::SchemaValidator::fromString(
            "services_array", schemas::SERVICES_ARRAY_SCHEMA
        );

        auto validationErrors = validator.validate(servicesJson);
        if (!validationErrors.empty()) {
            std::string errorMessage = "Schema validation failed:\n";
            for (const auto& error : validationErrors) {
                errorMessage += "  - Field: " + error.field + ", Error: " + error.message + "\n";
            }
            throw std::runtime_error(errorMessage);
        }

        // Load validated services into list
        if (!servicesJson.isArray()) {
            throw std::runtime_error("Configuration file must contain an array of services");
        }

        for (const auto& serviceJson : servicesJson) {
            try {
                std::string serviceName = serviceJson["name"].asString();
                std::string servicePath = serviceJson["path"].asString();
                int serviceInstances = serviceJson["instances"].asInt();

                // Create service object and add to list
                loadedServices.emplace_back(serviceName, servicePath, serviceInstances);
                
            } catch (const std::exception& e) {
                throw std::runtime_error("Error loading service: " + std::string(e.what()));
            }
        }

        std::cout << "Successfully loaded " << loadedServices.size() << " services from " 
                  << configFile.filename().string() << std::endl;
    }

    const Service* Services::findServiceByName(const std::string& name) const {
        auto it = std::find_if(loadedServices.begin(), loadedServices.end(),
            [&name](const Service& svc) {
                return svc.getName() == name;
            });
        
        return it != loadedServices.end() ? &(*it) : nullptr;
    }

} // loader