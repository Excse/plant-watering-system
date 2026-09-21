#include "components/base.h"

#include <utility>

#include "components/sensor.h"

namespace rhadar {

std::optional<ValidationError> validate(const Entity& entity) {
    if (!entity.unique_id() || entity.unique_id()->empty()) {
        return ValidationError{
            ValidationErrorCode::MissingUniqueId, "unique_id",
            "unique_id must not be empty"
        };
    }

    if (entity.message_expiry_interval() && *entity.message_expiry_interval() < 0) {
        return ValidationError{
            ValidationErrorCode::NegativeValue, "message_expiry_interval",
            "message_expiry_interval must be nonnegative"
        };
    }

    if (entity.entity_category() &&
        *entity.entity_category() != EntityCategory::Config &&
        *entity.entity_category() != EntityCategory::Diagnostic) {
        return ValidationError{
            ValidationErrorCode::InvalidEntityCategory, "entity_category",
            "entity_category must be CONFIG or DIAGNOSTIC when set"
        };
    }

    if (entity.availability_topic() && entity.availability_topic()->empty()) {
        return ValidationError{
            ValidationErrorCode::InvalidAvailabilityTopic, "availability_topic",
            "availability_topic must not be empty when set"
        };
    }

    return std::nullopt;
}

template <typename Derived, typename Config>
EntityBuilder<Derived, Config>::EntityBuilder(std::string unique_id) {
    _config._unique_id = std::move(unique_id);
}

template <typename Derived, typename Config>
Derived& EntityBuilder<Derived, Config>::entity_picture(std::string value) {
    _config._entity_picture = std::move(value);
    return static_cast<Derived&>(*this);
}

template <typename Derived, typename Config>
Derived& EntityBuilder<Derived, Config>::enabled_by_default(bool value) {
    _config._enabled_by_default = value;
    return static_cast<Derived&>(*this);
}

template <typename Derived, typename Config>
Derived& EntityBuilder<Derived, Config>::entity_category(EntityCategory value) {
    _config._entity_category = value;
    return static_cast<Derived&>(*this);
}

template <typename Derived, typename Config>
Derived& EntityBuilder<Derived, Config>::icon(std::string value) {
    _config._icon = std::move(value);
    return static_cast<Derived&>(*this);
}

template <typename Derived, typename Config>
Derived& EntityBuilder<Derived, Config>::json_attributes_topic(std::string value) {
    _config._json_attributes_topic = std::move(value);
    return static_cast<Derived&>(*this);
}

template <typename Derived, typename Config>
Derived& EntityBuilder<Derived, Config>::json_attributes_template(std::string value) {
    _config._json_attributes_template = std::move(value);
    return static_cast<Derived&>(*this);
}

template <typename Derived, typename Config>
Derived& EntityBuilder<Derived, Config>::default_entity_id(std::string value) {
    _config._default_entity_id = std::move(value);
    return static_cast<Derived&>(*this);
}

template <typename Derived, typename Config>
Derived& EntityBuilder<Derived, Config>::message_expiry_interval(int value) {
    _config._message_expiry_interval = value;
    return static_cast<Derived&>(*this);
}

template <typename Derived, typename Config>
Derived& EntityBuilder<Derived, Config>::visible_by_default(bool value) {
    _config._visible_by_default = value;
    return static_cast<Derived&>(*this);
}

template <typename Derived, typename Config>
Derived& EntityBuilder<Derived, Config>::availability_topic(std::string value) {
    _config._availability_topic = std::move(value);
    return static_cast<Derived&>(*this);
}

template class EntityBuilder<SensorBuilder, Sensor>;

} // namespace rhadar
