//
// Created by rdias on 3/12/26.
//

#include "services.h"

#include <utility>

namespace loader {
    std::filesystem::path services::getPath() {
        return path;
    }

    std::string services::getName() {
        return name;
    }

    services::services(std::string name, std::filesystem::path path, const int instances):
        name(std::move(name)), path(std::move(path)), instances(instances) {

    }

} // loader