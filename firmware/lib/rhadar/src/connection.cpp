#include "connection.h"

#include <utility>

namespace rhadar {

std::optional<ValidationError> validate(const Connection& connection) {
    if (connection.type().empty()) {
        return ValidationError{
            ValidationErrorCode::MISSING_CONNECTION_TYPE, "type",
            "connection type must not be empty"
        };
    }

    if (connection.identifier().empty()) {
        return ValidationError{
            ValidationErrorCode::MISSING_CONNECTION_IDENTIFIER, "identifier",
            "connection identifier must not be empty"
        };
    }

    return std::nullopt;
}

ConnectionBuilder::ConnectionBuilder(std::string type, std::string identifier) {
    _config._type = std::move(type);
    _config._identifier = std::move(identifier);
}

ConnectionBuilder& ConnectionBuilder::type(std::string value) {
    _config._type = std::move(value);
    return *this;
}

ConnectionBuilder& ConnectionBuilder::identifier(std::string value) {
    _config._identifier = std::move(value);
    return *this;
}

Result<Connection> ConnectionBuilder::build() const {
    if (auto error = validate(_config)) {
        return std::unexpected(std::move(*error));
    }

    return _config;
}

} // namespace rhadar
