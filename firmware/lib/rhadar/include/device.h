#pragma once

#include <optional>
#include <string>
#include <vector>

#include "result.h"
#include "connection.h"

namespace rhadar {

// Code Source: https://github.com/home-assistant/core/blob/2026.9.2/homeassistant/components/mqtt/schemas.py#L131
// Documentation Source: NaN
#define DEVICE_FIELDS(X, TargetEnum)                                      \
    X(TargetEnum, Identifiers,      "identifiers",       "ids")          \
    X(TargetEnum, Name,             "name",              "name")         \
    X(TargetEnum, SuggestedArea,    "suggested_area",    "sa")           \
    X(TargetEnum, SerialNumber,     "serial_number",     "sn")           \
    X(TargetEnum, ConfigurationUrl, "configuration_url", "cu")           \
    X(TargetEnum, Connections,      "connections",       "cns")          \
    X(TargetEnum, Manufacturer,     "manufacturer",      "mf")           \
    X(TargetEnum, Model,            "model",             "mdl")          \
    X(TargetEnum, ModelId,          "model_id",          "mdl_id")       \
    X(TargetEnum, SwVersion,        "sw_version",        "sw")           \
    X(TargetEnum, HwVersion,        "hw_version",        "hw")
DEFINE_ABBREVIATED_ENUM(DeviceFields, DEVICE_FIELDS)

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
    std::vector<std::string> _identifiers;
    std::string _name;
    std::optional<std::string> _suggested_area;
    std::optional<std::string> _serial_number;
    std::optional<std::string> _configuration_url;
    std::vector<Connection> _connections;
    std::optional<std::string> _manufacturer;
    std::optional<std::string> _model;
    std::optional<std::string> _model_id;
    std::optional<std::string> _sw_version;
    std::optional<std::string> _hw_version;

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

    [[nodiscard]] DeviceBuilder& add_connection(std::string type, std::string identifier);

    [[nodiscard]] Result<Device> build() const;

private:
    Device _config;
};

} // namespace rhadar
