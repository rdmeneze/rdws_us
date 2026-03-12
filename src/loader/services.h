#ifndef RDWS_US_SERVICES_H
#define RDWS_US_SERVICES_H

#include <filesystem>
#include <string>


namespace loader {
    class services {
        private:
            std::string name;
            std::filesystem::path path;
            int instances;

        public:
            services(std::string name, std::filesystem::path path, int instances);
            ~services()= default;

            std::filesystem::path getPath();
            std::string getName();
    };
} // loader

#endif //RDWS_US_SERVICES_H