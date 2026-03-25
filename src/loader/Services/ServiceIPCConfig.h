
#pragma once

#include <string>
#include <stdexcept>
#include <cstdint>
#include <array>
#include <utility>

namespace loader {
  /**
   * @class IPCType
   * @brief Represents the type of IPC mechanism used for service communication.
   */
  class IPCType {
    public:
    /**
     * @enum Type
     * @brief type of communication used by the service
     */
    enum Type {
        UNIX_SOCKET,
        TCP_SOCKET
    };

    explicit constexpr IPCType(Type v) : type(v) {};

    explicit constexpr operator Type() const { return type; }

    [[nodiscard]] std::string toString() const {
      switch (type) {
        case UNIX_SOCKET ... TCP_SOCKET:
          return std::string(validTypes[static_cast<size_t>(type)]);
        default: return "UNKNOWN";
      }
    }

    private:
      Type type;
      const std::array<std::string_view, 2> validTypes = {"UNIX_SOCKET", "TCP_SOCKET"};
  };

  class ServiceIPCConfig {
    public:
      ServiceIPCConfig(IPCType type, std::string  baseEndpoint)
        : type(std::move(type)), baseEndpoint(std::move(baseEndpoint)) {}

    [[nodiscard]] std::string generateInstanceEndpoint(const int instanceId) const {
      switch (static_cast<IPCType::Type>(type)) {
        case IPCType::UNIX_SOCKET: {
          // /tmp/service.sock -> /tmp/service_0.sock
          const auto pos = baseEndpoint.find_last_of('.');
          if (pos == std::string::npos) {
            throw std::runtime_error("Invalid UNIX socket format: " + baseEndpoint);
          }
          return baseEndpoint.substr(0, pos) +
                 "_" +
                 std::to_string(instanceId) +
                 baseEndpoint.substr(pos);
        }
        case IPCType::TCP_SOCKET: {
          // localhost:8001 -> localhost:8001, localhost:8002, ...
          auto pos = baseEndpoint.find_last_of(':');
          if (pos == std::string::npos) {
            throw std::runtime_error("Invalid TCP socket format: " + baseEndpoint);
          }
          const std::string host = baseEndpoint.substr(0, pos);
          uint16_t port = static_cast<uint16_t>(std::stoi(baseEndpoint.substr(pos + 1)));
          return host + ":" + std::to_string(port + instanceId);
        }
        default:
          throw std::runtime_error("Unsupported IPC type: " + type.toString());
      }
    }

    private:
      IPCType type;             // UNIX_SOCKET, TCP_SOCKET
      std::string baseEndpoint; // "/tmp/service.sock" ou "localhost:8001"
  };
}