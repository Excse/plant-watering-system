#include "origin.h"

#include <utility>

namespace rhadar {

std::optional<ValidationError> validate(const Origin& origin) {
    if (origin.name().empty()) {
        return ValidationError{
            ValidationErrorCode::MISSING_ORIGIN_NAME, "name",
            "origin name must not be empty"
        };
    }

    return std::nullopt;
}

OriginBuilder::OriginBuilder(std::string name) {
    _config._name = std::move(name);
}

OriginBuilder& OriginBuilder::name(std::string value) {
    _config._name = std::move(value);
    return *this;
}

OriginBuilder& OriginBuilder::sw_version(std::string value) {
    _config._sw_version = std::move(value);
    return *this;
}

OriginBuilder& OriginBuilder::support_url(std::string value) {
    _config._support_url = std::move(value);
    return *this;
}

Result<Origin> OriginBuilder::build() const {
    if (auto error = validate(_config)) {
        return std::unexpected(std::move(*error));
    }

    return _config;
}

} // namespace rhadar
