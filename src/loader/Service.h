
#ifndef RDWS_US_SERVICE_H
#define RDWS_US_SERVICE_H

#include <filesystem>
#include <string>
#include <utility>

namespace loader {

    struct Service {
        Service(std::string name, std::filesystem::path path, const int instances):
                name(std::move(name)), path(std::move(path)), instances(instances) {};

        ~Service() = default;

        [[nodiscard]] std::filesystem::path getPath() const { return path; };
        [[nodiscard]] std::string getName() const { return name; };
        [[nodiscard]] int getInstances() const { return instances; };
        private:
            std::string name;
            std::filesystem::path path;
            int instances;
    };
} // loader

#endif