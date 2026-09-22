#pragma once

#include <optional>
#include <string>

#include "result.h"

namespace rhadar {

// Code Source: https://github.com/home-assistant/core/blob/2026.9.2/homeassistant/components/mqtt/schemas.py#L138C13-L140C15
// Documentation Source: NaN
class Connection final {
public:
    [[nodiscard]] const std::string& type() const noexcept { return _type; }

    [[nodiscard]] const std::string& identifier() const noexcept { return _identifier; }

private:
    std::string _type;       // e.g. "mac"
    std::string _identifier; // e.g. "02:5b:26:a8:dc:12"

    friend class ConnectionBuilder;
    friend class DeviceBuilder;
};

[[nodiscard]] std::optional<ValidationError> validate(const Connection& connection);

class ConnectionBuilder {
public:
    ConnectionBuilder(std::string type, std::string identifier);

    [[nodiscard]] ConnectionBuilder& type(std::string value);

    [[nodiscard]] ConnectionBuilder& identifier(std::string value);

    [[nodiscard]] Result<Connection> build() const;

private:
    Connection _config;
};

} // namespace rhadar
