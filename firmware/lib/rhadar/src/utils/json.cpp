#include "utils/json.h"

#include <chrono>
#include <cstdint>
#include <optional>
#include <string_view>
#include <type_traits>
#include <utility>

#include "message.h"

namespace rhadar {

class JsonObject {
public:
    explicit JsonObject(EnumStringFormat key_format = EnumStringFormat::Full) 
        : _key_format(key_format) {}

    void string(std::string_view key, std::string_view value) {
        key_prefix(key);
        quoted(value);
    }

    template <typename E>
        requires std::is_enum_v<E>
    void string(E key, std::string_view value) {
        string(to_string(key, _key_format), value);
    }

    template <typename Key, typename T>
        requires (std::is_same_v<T, std::string> || std::is_enum_v<T>)
    void string(const Key& key, const std::optional<T>& value) {
        if (!value) return;
        if constexpr (std::is_enum_v<T>) {
            string(key, to_string(*value));
        } else {
            string(key, *value);
        }
    }

    void boolean(std::string_view key, bool value) {
        key_prefix(key);
        _json += value ? "true" : "false";
    }

    template <typename E>
        requires std::is_enum_v<E>
    void boolean(E key, bool value) {
        boolean(to_string(key, _key_format), value);
    }

    template <typename Key>
    void boolean(const Key& key, const std::optional<bool>& value) {
        if (value) boolean(key, *value);
    }

    void integer(std::string_view key, std::int64_t value) {
        key_prefix(key);
        _json += std::to_string(value);
    }

    template <typename E>
        requires std::is_enum_v<E>
    void integer(E key, std::int64_t value) {
        integer(to_string(key, _key_format), value);
    }

    template <typename Key, typename T>
        requires std::is_integral_v<T>
    void integer(const Key& key, const std::optional<T>& value) {
        if (value) integer(key, static_cast<std::int64_t>(*value));
    }

    template <typename Key, typename Rep, typename Period>
        requires std::is_integral_v<Rep>
    void integer(const Key& key, const std::optional<std::chrono::duration<Rep, Period>>& value) {
        if (value) integer(key, static_cast<std::int64_t>(value->count()));
    }

    void object(std::string_view key, std::string value) {
        key_prefix(key);
        _json += std::move(value);
    }

    template <typename E>
        requires std::is_enum_v<E>
    void object(E key, std::string value) {
        object(to_string(key, _key_format), std::move(value));
    }

    void string_array(std::string_view key, const std::vector<std::string>& values) {
        key_prefix(key);
        _json += '[';
        bool first = true;
        for (const auto& value : values) {
            if (!first) _json += ',';
            first = false;
            quoted(value);
        }
        _json += ']';
    }

    template <typename E>
        requires std::is_enum_v<E>
    void string_array(E key, const std::vector<std::string>& values) {
        string_array(to_string(key, _key_format), values);
    }

    void connections(std::string_view key, const std::vector<Connection>& values) {
        key_prefix(key);
        _json += '[';
        bool first = true;
        for (const auto& connection : values) {
            if (!first) _json += ',';
            first = false;
            _json += '[';
            quoted(connection.type());
            _json += ',';
            quoted(connection.identifier());
            _json += ']';
        }
        _json += ']';
    }

    template <typename E>
        requires std::is_enum_v<E>
    void connections(E key, const std::vector<Connection>& values) {
        connections(to_string(key, _key_format), values);
    }

    [[nodiscard]] std::string finish() && {
        _json += '}';
        return std::move(_json);
    }

private:
    void key_prefix(std::string_view key) {
        if (!_first) _json += ',';
        _first = false;
        quoted(key);
        _json += ':';
    }

    void quoted(std::string_view value) {
        static constexpr char HEX[] = "0123456789abcdef";
        _json += '"';
        for (const unsigned char character : value) {
            switch (character) {
                case '"': _json += "\\\""; break;
                case '\\': _json += "\\\\"; break;
                case '\b': _json += "\\b"; break;
                case '\f': _json += "\\f"; break;
                case '\n': _json += "\\n"; break;
                case '\r': _json += "\\r"; break;
                case '\t': _json += "\\t"; break;
                default:
                    if (character < 0x20) {
                        _json += "\\u00";
                        _json += HEX[character >> 4];
                        _json += HEX[character & 0x0f];
                    } else {
                        _json += static_cast<char>(character);
                    }
            }
        }
        _json += '"';
    }

    std::string _json = "{";
    bool _first = true;
    EnumStringFormat _key_format;
};

std::string serialize_device(JsonObject& json, const Device& value) {
    JsonObject json;
    if (!value.identifiers().empty()) {
        json.string_array(DeviceFields::Identifiers, value.identifiers());
    }
    if (!value.name().empty()) json.string(DeviceFields::Name, value.name());
    json.string(DeviceFields::SuggestedArea, value.suggested_area());
    json.string(DeviceFields::SerialNumber, value.serial_number());
    json.string(DeviceFields::ConfigurationUrl, value.configuration_url());
    if (!value.connections().empty()) {
        json.connections(DeviceFields::Connections, value.connections());
    }
    json.string(DeviceFields::Manufacturer, value.manufacturer());
    json.string(DeviceFields::Model, value.model());
    json.string(DeviceFields::ModelId, value.model_id());
    json.string(DeviceFields::SwVersion, value.sw_version());
    json.string(DeviceFields::HwVersion, value.hw_version());
    return std::move(json).finish();
}

std::string serialize_origin(const Origin& value) {
    JsonObject json;
    json.string(OriginFields::Name, value.name());
    json.string(OriginFields::SwVersion, value.sw_version());
    json.string(OriginFields::SupportUrl, value.support_url());
    return std::move(json).finish();
}

std::string serialize_component(const Sensor& value) {
    JsonObject json;
    json.string(ComponentFields::Platform, "sensor");
    json.string(EntityFields::UniqueId, value.unique_id());
    json.string(SensorFields::StateTopic, value.state_topic());
    json.string(SensorFields::Name, value.name());
    json.string(SensorFields::ValueTemplate, value.value_template());
    json.string(SensorFields::DeviceClass, value.device_class());
    json.integer(SensorFields::ExpireAfter, value.expire_after());
    json.boolean(SensorFields::ForceUpdate, value.force_update());
    json.string(SensorFields::LastResetValueTemplate, value.last_reset_value_template());
    json.string_array(SensorFields::Options, value.options());
    json.integer(SensorFields::SuggestedDisplayPrecision, value.suggested_display_precision());
    json.string(SensorFields::StateClass, value.state_class());
    json.string(SensorFields::UnitOfMeasurement, value.unit_of_measurement());
    json.string(EntityFields::EntityPicture, value.entity_picture());
    json.boolean(EntityFields::EnabledByDefault, value.enabled_by_default());
    json.string(EntityFields::EntityCategory, value.entity_category());
    json.string(EntityFields::Icon, value.icon());
    json.string(EntityFields::JsonAttributesTopic, value.json_attributes_topic());
    json.string(EntityFields::JsonAttributesTemplate, value.json_attributes_template());
    json.string(EntityFields::DefaultEntityId, value.default_entity_id());
    json.integer(EntityFields::MessageExpiryInterval, value.message_expiry_interval());
    json.boolean(EntityFields::VisibleByDefault, value.visible_by_default());
    json.string(EntityFields::AvailabilityTopic, value.availability_topic());
    return std::move(json).finish();
}

std::string serialize_component(const ComponentEntity& entity) {
    return std::visit(
        [](const auto& value) { return serialize_component(value); },
        entity
    );
}

std::string serialize_components(const std::vector<Component>& values) {
    JsonObject json;
    for (const auto& value : values) {
        json.object(value.id(), serialize_component(value.entity()));
    }
    return std::move(json).finish();
}

std::string serialize_message_payload(
    const Device& device,
    const Origin& origin,
    const std::vector<Component>& components
) {
    JsonObject payload;
    payload.object(MessageFields::Device, serialize_device(device));
    payload.object(MessageFields::Origin, serialize_origin(origin));
    payload.object(MessageFields::Components, serialize_components(components));
    return std::move(payload).finish();
}

} // namespace rhadar
