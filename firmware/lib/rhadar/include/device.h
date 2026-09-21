#pragma once

#include <optional>
#include <string>
#include <vector>

#include "result.h"
#include "connection.h"

namespace rhadar {

// Code Source: https://github.com/home-assistant/core/blob/2026.9.2/homeassistant/components/mqtt/schemas.py#L131
// Documentation Source: NaN
class Device final {
public:
    [[nodiscard]] const std::vector<std::string>& identifiers() const noexcept { return _identifiers; }

    [[nodiscard]] const std::string& name() const noexcept { return _name; }

    [[nodiscard]] const std::optional<std::string>& suggested_area() const noexcept { return _suggested_area; }

    [[nodiscard]] const std::optional<std::string>& serial_number() const noexcept { return _serial_number; }

    [[nodiscard]] const std::optional<std::string>& configuration_url() const noexcept { return _configuration_url; }

    [[nodiscard]] const std::vector<Connection>& connections() const noexcept { return _connections; }

    [[nodiscard]] const std::optional<std::string>& manufacturer() const noexcept { return _manufacturer; }

    [[nodiscard]] const std::optional<std::string>& model() const noexcept { return _model; }

    [[nodiscard]] const std::optional<std::string>& model_id() const noexcept { return _model_id; }

    [[nodiscard]] const std::optional<std::string>& sw_version() const noexcept { return _sw_version; }

    [[nodiscard]] const std::optional<std::string>& hw_version() const noexcept { return _hw_version; }

private:
    std::vector<std::string> _identifiers; // identifiers or ids
    std::string _name; // name
    std::optional<std::string> _suggested_area; // suggested_area or sa
    std::optional<std::string> _serial_number; // serial_number or sn
    std::optional<std::string> _configuration_url; // configuration_url or cu
    std::vector<Connection> _connections; // connections or cns
    std::optional<std::string> _manufacturer; // manufacturer or mf
    std::optional<std::string> _model; // model or mdl
    std::optional<std::string> _model_id; // model_id or mdl_id
    std::optional<std::string> _sw_version; // sw_version or sw
    std::optional<std::string> _hw_version; // hw_version or hw

    friend class DeviceBuilder;
};

[[nodiscard]] std::optional<ValidationError> validate(const Device& device);

class DeviceBuilder {
public:
    explicit DeviceBuilder(std::string identifier);

    explicit DeviceBuilder(Connection connection);

    [[nodiscard]] DeviceBuilder& identifiers(std::vector<std::string> value);

    [[nodiscard]] DeviceBuilder& name(std::string value);

    [[nodiscard]] DeviceBuilder& suggested_area(std::string value);

    [[nodiscard]] DeviceBuilder& serial_number(std::string value);

    [[nodiscard]] DeviceBuilder& configuration_url(std::string value);

    [[nodiscard]] DeviceBuilder& connections(std::vector<Connection> value);

    [[nodiscard]] DeviceBuilder& manufacturer(std::string value);

    [[nodiscard]] DeviceBuilder& model(std::string value);

    [[nodiscard]] DeviceBuilder& model_id(std::string value);

    [[nodiscard]] DeviceBuilder& sw_version(std::string value);

    [[nodiscard]] DeviceBuilder& hw_version(std::string value);

    [[nodiscard]] DeviceBuilder& add_identifier(std::string value);

    [[nodiscard]] DeviceBuilder& add_connection(Connection value);

    [[nodiscard]] Result<Device> build() const;

private:
    Device _config;
};

} // namespace rhadar
