#include "device.h"

#include <utility>

namespace rhadar {

std::optional<ValidationError> validate(const Device& device) {
    if (device.identifiers().empty() && device.connections().empty()) {
        return ValidationError{
            ValidationErrorCode::MissingDeviceIdentity, 
            "identifiers",
            "device requires at least one identifier or connection"
        };
    }

    for (std::size_t index = 0; index < device.identifiers().size(); ++index) {
        if (device.identifiers()[index].empty()) {
            return ValidationError{
                ValidationErrorCode::EmptyDeviceIdentifier,
                "identifiers[" + std::to_string(index) + "]",
                "device identifier must not be empty"
            };
        }
    }

    for (std::size_t index = 0; index < device.connections().size(); ++index) {
        if (auto error = validate(device.connections()[index])) {
            return ValidationError {
                error->code,
                "connections[" + std::to_string(index) + "]." + error->field,
                error->message,
            };
        }
    }

    return std::nullopt;
}

DeviceBuilder::DeviceBuilder(std::string identifier) {
    _config._identifiers.push_back(std::move(identifier));
}

DeviceBuilder::DeviceBuilder(Connection connection) {
    _config._connections.push_back(std::move(connection));
}

DeviceBuilder& DeviceBuilder::identifiers(std::vector<std::string> value) {
    _config._identifiers = std::move(value);
    return *this;
}

DeviceBuilder& DeviceBuilder::name(std::string value) {
    _config._name = std::move(value);
    return *this;
}

DeviceBuilder& DeviceBuilder::suggested_area(std::string value) {
    _config._suggested_area = std::move(value);
    return *this;
}

DeviceBuilder& DeviceBuilder::serial_number(std::string value) {
    _config._serial_number = std::move(value);
    return *this;
}

DeviceBuilder& DeviceBuilder::configuration_url(std::string value) {
    _config._configuration_url = std::move(value);
    return *this;
}

DeviceBuilder& DeviceBuilder::connections(std::vector<Connection> value) {
    _config._connections = std::move(value);
    return *this;
}

DeviceBuilder& DeviceBuilder::manufacturer(std::string value) {
    _config._manufacturer = std::move(value);
    return *this;
}

DeviceBuilder& DeviceBuilder::model(std::string value) {
    _config._model = std::move(value);
    return *this;
}

DeviceBuilder& DeviceBuilder::model_id(std::string value) {
    _config._model_id = std::move(value);
    return *this;
}

DeviceBuilder& DeviceBuilder::sw_version(std::string value) {
    _config._sw_version = std::move(value);
    return *this;
}

DeviceBuilder& DeviceBuilder::hw_version(std::string value) {
    _config._hw_version = std::move(value);
    return *this;
}

DeviceBuilder& DeviceBuilder::add_identifier(std::string value) {
    _config._identifiers.push_back(std::move(value));
    return *this;
}

DeviceBuilder& DeviceBuilder::add_connection(Connection value) {
    _config._connections.push_back(std::move(value));
    return *this;
}

DeviceBuilder& DeviceBuilder::add_connection(std::string type, std::string identifier) {
    Connection connection;
    connection._type = std::move(type);
    connection._identifier = std::move(identifier);
    _config._connections.push_back(std::move(connection));
    return *this;
}

Result<Device> DeviceBuilder::build() const {
    if (auto error = validate(_config)) {
        return std::unexpected(std::move(*error));
    }

    return _config;
}

} // namespace rhadar
