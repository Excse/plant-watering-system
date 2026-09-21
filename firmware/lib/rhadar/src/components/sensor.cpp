#include "components/sensor.h"

#include <utility>

namespace rhadar {

std::optional<ValidationError> validate(const Sensor& sensor) {
    if (auto error = validate(static_cast<const Entity&>(sensor))) {
        return error;
    }

    if (sensor.state_topic().empty()) {
        return ValidationError{
            ValidationErrorCode::MissingStateTopic, "state_topic",
            "state_topic must not be empty"
        };
    }

    if (sensor.expire_after() && sensor.expire_after()->count() < 0) {
        return ValidationError{
            ValidationErrorCode::NegativeValue, "expire_after",
            "expire_after must be nonnegative"
        };
    }
    
    if (sensor.suggested_display_precision() && *sensor.suggested_display_precision() < 0) {
        return ValidationError{
            ValidationErrorCode::NegativeValue, "suggested_display_precision",
            "suggested_display_precision must be nonnegative"
        };
    }

    if (sensor.entity_category() && *sensor.entity_category() != EntityCategory::Diagnostic) {
        return ValidationError{
            ValidationErrorCode::InvalidEntityCategory, "entity_category",
            "sensor entity_category must be DIAGNOSTIC when set"};
    }

    if (!sensor.options().empty() && (sensor.device_class() != SensorDeviceClass::Enum || sensor.state_class() || sensor.unit_of_measurement())) {
        return ValidationError{
            ValidationErrorCode::IncompatibleOptions, "options",
            "options require device_class ENUM and no state_class or unit_of_measurement"
        };
    }

    if (sensor.last_reset_value_template() && sensor.state_class() != SensorStateClass::Total) {
        return ValidationError{
            ValidationErrorCode::IncompatibleLastReset, "last_reset_value_template",
            "last_reset_value_template requires state_class TOTAL"
        };
    }

    if (sensor.state_class() == SensorStateClass::MeasurementAngle && sensor.unit_of_measurement() != "\u00b0") {
        return ValidationError{
            ValidationErrorCode::IncompatibleStateClassUnit, "unit_of_measurement",
            "state_class MEASUREMENT_ANGLE requires degree units (\u00b0)"};
    }

    return std::nullopt;
}

SensorBuilder::SensorBuilder(std::string unique_id)
    : EntityBuilder(std::move(unique_id)) {}

SensorBuilder& SensorBuilder::state_topic(std::string value) {
    _config._state_topic = std::move(value);
    return *this;
}

SensorBuilder& SensorBuilder::value_template(std::string value) {
    _config._value_template = std::move(value);
    return *this;
}

SensorBuilder& SensorBuilder::name(std::string value) {
    _config._name = std::move(value);
    return *this;
}

SensorBuilder& SensorBuilder::device_class(SensorDeviceClass value) {
    _config._device_class = value;
    return *this;
}

SensorBuilder& SensorBuilder::state_class(SensorStateClass value) {
    _config._state_class = value;
    return *this;
}

SensorBuilder& SensorBuilder::unit_of_measurement(std::string value) {
    _config._unit_of_measurement = std::move(value);
    return *this;
}

SensorBuilder& SensorBuilder::expire_after(std::chrono::seconds value) {
    _config._expire_after = value;
    return *this;
}

SensorBuilder& SensorBuilder::suggested_display_precision(int value) {
    _config._suggested_display_precision = value;
    return *this;
}

SensorBuilder& SensorBuilder::force_update(bool value) {
    _config._force_update = value;
    return *this;
}

SensorBuilder& SensorBuilder::last_reset_value_template(std::string value) {
    _config._last_reset_value_template = std::move(value);
    return *this;
}

SensorBuilder& SensorBuilder::options(std::vector<std::string> values) {
    _config._options = std::move(values);
    return *this;
}

SensorBuilder& SensorBuilder::add_option(std::string value) {
    _config._options.push_back(std::move(value));
    return *this;
}

Result<Sensor> SensorBuilder::build() const {
    if (auto error = validate(_config)) {
        return std::unexpected(std::move(*error));
    }

    return _config;
}

} // namespace rhadar
