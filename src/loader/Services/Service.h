
#ifndef RDWS_US_SERVICE_H
#define RDWS_US_SERVICE_H

#include <filesystem>
#include <string>
#include <utility>

namespace loader {

    class Service {
    public:
        Service(std::string name, std::filesystem::path path, const int instances, bool active = false):
                name(std::move(name)), path(std::move(path)), instances(instances), active(active) {};

        ~Service() = default;

        [[nodiscard]] std::filesystem::path getPath() const { return path; };
        [[nodiscard]] std::string getName() const { return name; };
        [[nodiscard]] int getInstances() const { return instances; };
        [[nodiscard]] bool isActive() const { return active; };
        
    private:
        std::string name;
        std::filesystem::path path;
        int instances;
        bool active;
    };
} // loader

#endif