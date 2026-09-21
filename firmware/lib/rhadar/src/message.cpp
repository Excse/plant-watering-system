#include "message.h"

#include <utility>

#include "utils/json.h"

namespace rhadar {

bool valid_topic_id(const std::string& value) {
    if (value.empty()) return false;
    for (const unsigned char character : value) {
        const bool alphanumeric =
            (character >= 'a' && character <= 'z') ||
            (character >= 'A' && character <= 'Z') ||
            (character >= '0' && character <= '9');
        if (!alphanumeric && character != '_' && character != '-') return false;
    }
    return true;
}

bool valid_discovery_prefix(const std::string& value) {
    return !value.empty() && value.front() != '/' && value.back() != '/' &&
           value.find('+') == std::string::npos &&
           value.find('#') == std::string::npos &&
           value.find('\0') == std::string::npos;
}

std::optional<ValidationError> validate_component(const ComponentEntity& entity) {
    return std::visit(
        [](const auto& value) { return validate(value); },
        entity
    );
}

Component::Component(std::string id, ComponentEntity entity)
    : _id(std::move(id)), _entity(std::move(entity)) {}

MessageBuilder::MessageBuilder(Device device, Origin origin, std::string object_id) 
    : _device(std::move(device)), _origin(std::move(origin)), _object_id(std::move(object_id)) {}

MessageBuilder& MessageBuilder::discovery_prefix(std::string value) {
    _discovery_prefix = std::move(value);
    return *this;
}

MessageBuilder& MessageBuilder::node_id(std::string value) {
    _node_id = std::move(value);
    return *this;
}

MessageBuilder& MessageBuilder::components(std::vector<Component> value) {
    _components = std::move(value);
    return *this;
}

MessageBuilder& MessageBuilder::add_component(Component value) {
    _components.push_back(std::move(value));
    return *this;
}

MessageBuilder& MessageBuilder::add_component(std::string id, ComponentEntity entity) {
    _components.emplace_back(std::move(id), std::move(entity));
    return *this;
}

MessageBuilder& MessageBuilder::qos(int value) {
    _qos = value;
    return *this;
}

Result<Message> MessageBuilder::build() const {
    if (auto error = validate(_device)) {
        return std::unexpected(ValidationError{
            error->code,
            "device." + error->field,
            error->message,
        });
    }

    if (auto error = validate(_origin)) {
        return std::unexpected(ValidationError{
            error->code,
            "origin." + error->field,
            error->message,
        });
    }

    if (!valid_discovery_prefix(_discovery_prefix)) {
        return std::unexpected(ValidationError{
            ValidationErrorCode::InvalidDiscoveryPrefix, "discovery_prefix",
            "discovery_prefix must be a nonempty MQTT topic prefix without wildcards"
        });
    }

    if (!valid_topic_id(_object_id)) {
        return std::unexpected(ValidationError{
            ValidationErrorCode::InvalidObjectId, "object_id",
            "object_id may contain only letters, digits, underscores, and hyphens"
        });
    }

    if (_node_id && !valid_topic_id(*_node_id)) {
        return std::unexpected(ValidationError{
            ValidationErrorCode::InvalidNodeId, "node_id",
            "node_id may contain only letters, digits, underscores, and hyphens"
        });
    }

    if (_qos < 0 || _qos > 2) {
        return std::unexpected(ValidationError{
            ValidationErrorCode::InvalidQos, "qos", 
            "qos must be 0, 1, or 2"
        });
    }

    if (_components.empty()) {
        return std::unexpected(ValidationError{
            ValidationErrorCode::MissingComponents, "components",
            "at least one discovery component is required"
        });
    }

    for (std::size_t index = 0; index < _components.size(); ++index) {
        const auto& component = _components[index];

        if (!valid_topic_id(component.id())) {
            return std::unexpected(ValidationError{
                ValidationErrorCode::InvalidComponentId,
                "components[" + std::to_string(index) + "].id",
                "component id may contain only letters, digits, underscores, and hyphens"
            });
        }

        for (std::size_t previous = 0; previous < index; ++previous) {
            if (_components[previous].id() == component.id()) {
                return std::unexpected(ValidationError{
                    ValidationErrorCode::DuplicateComponentId,
                    "components[" + std::to_string(index) + "].id",
                    "component ids must be unique"
                });
            }
        }

        if (auto error = validate_component(component.entity())) {
            return std::unexpected(ValidationError{
                error->code,
                "components[" + std::to_string(index) + "]." + error->field,
                error->message,
            });
        }
    }

    Message message;
    message._topic = _discovery_prefix + "/device/";
    if (_node_id) {
        message._topic += *_node_id + '/';
    }
    message._topic += _object_id + "/config";
    message._payload = rhadar::serialize_message_payload(_device, _origin, _components);
    message._qos = _qos;
    return message;
}

} // namespace rhadar
