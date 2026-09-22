#pragma once

#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "components/sensor.h"
#include "device.h"
#include "origin.h"
#include "result.h"

namespace rhadar {

using ComponentEntity = std::variant<Sensor>;

// Code Source: NaN
// Documentation Source: NaN
#define COMPONENT_FIELDS(X, TargetEnum)           \
    X(TargetEnum, Platform, "platform", "p")
DEFINE_ABBREVIATED_ENUM(ComponentFields, COMPONENT_FIELDS)

class Component final {
public:
    Component(std::string id, ComponentEntity entity);

    [[nodiscard]] const std::string& id() const noexcept { return _id; }

    [[nodiscard]] const ComponentEntity& entity() const noexcept { return _entity; }

private:
    std::string _id;
    ComponentEntity _entity;
};

// Code Source: NaN
// Documentation Source: NaN
#define MESSAGE_FIELDS(X, TargetEnum)                  \
    X(TargetEnum, Device,     "device",     "dev")     \
    X(TargetEnum, Origin,     "origin",     "o")       \
    X(TargetEnum, Components, "components", "cmps")
DEFINE_ABBREVIATED_ENUM(MessageFields, MESSAGE_FIELDS)

class Message final {
public:
    [[nodiscard]] const std::string& topic() const noexcept { return _topic; }
    
    [[nodiscard]] const std::string& payload() const noexcept { return _payload; }

    [[nodiscard]] int qos() const noexcept { return _qos; }

    [[nodiscard]] bool retain() const noexcept { return _retain; }

private:
    std::string _topic;
    std::string _payload;
    int _qos = 0;
    bool _retain = true;

    friend class MessageBuilder;
};

class MessageBuilder {
public:
    MessageBuilder(Device device, Origin origin, std::string object_id);

    [[nodiscard]] MessageBuilder& discovery_prefix(std::string value);

    [[nodiscard]] MessageBuilder& node_id(std::string value);

    [[nodiscard]] MessageBuilder& components(std::vector<Component> value);

    [[nodiscard]] MessageBuilder& add_component(Component value);

    [[nodiscard]] MessageBuilder& add_component(std::string id, ComponentEntity entity);
    
    [[nodiscard]] MessageBuilder& qos(int value);

    [[nodiscard]] Result<Message> build() const;

private:
    Device _device;
    Origin _origin;
    std::string _object_id;
    std::string _discovery_prefix = "homeassistant";
    std::optional<std::string> _node_id;
    std::vector<Component> _components;
    int _qos = 0;
};

} // namespace rhadar
