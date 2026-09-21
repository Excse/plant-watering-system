#include "utils/json.h"

#include <cstdint>
#include <string_view>
#include <utility>

#include "message.h"

namespace rhadar::utils {
namespace {

class JsonObject {
public:
    void string(std::string_view key, std::string_view value) {
        key_prefix(key);
        quoted(value);
    }

    void boolean(std::string_view key, bool value) {
        key_prefix(key);
        _json += value ? "true" : "false";
    }

    void integer(std::string_view key, std::int64_t value) {
        key_prefix(key);
        _json += std::to_string(value);
    }

    void object(std::string_view key, std::string value) {
        key_prefix(key);
        _json += std::move(value);
    }

    void string_array(
        std::string_view key,
        const std::vector<std::string>& values
    ) {
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

    void connections(
        std::string_view key,
        const std::vector<Connection>& values
    ) {
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
};

std::string serialize_device(const Device& value) {
    JsonObject json;
    if (!value.identifiers().empty()) {
        json.string_array("identifiers", value.identifiers());
    }
    if (!value.name().empty()) json.string("name", value.name());
    if (value.suggested_area()) {
        json.string("suggested_area", *value.suggested_area());
    }
    if (value.serial_number()) {
        json.string("serial_number", *value.serial_number());
    }
    if (value.configuration_url()) {
        json.string("configuration_url", *value.configuration_url());
    }
    if (!value.connections().empty()) {
        json.connections("connections", value.connections());
    }
    if (value.manufacturer()) {
        json.string("manufacturer", *value.manufacturer());
    }
    if (value.model()) json.string("model", *value.model());
    if (value.model_id()) json.string("model_id", *value.model_id());
    if (value.sw_version()) json.string("sw_version", *value.sw_version());
    if (value.hw_version()) json.string("hw_version", *value.hw_version());
    return std::move(json).finish();
}

std::string serialize_origin(const Origin& value) {
    JsonObject json;
    json.string("name", value.name());
    if (value.sw_version()) json.string("sw_version", *value.sw_version());
    if (value.support_url()) json.string("support_url", *value.support_url());
    return std::move(json).finish();
}

std::string serialize_component(const Sensor& value) {
    JsonObject json;
    json.string("platform", "sensor");
    json.string("unique_id", *value.unique_id());
    json.string("state_topic", value.state_topic());
    if (value.name()) json.string("name", *value.name());
    if (value.value_template()) {
        json.string("value_template", *value.value_template());
    }
    if (value.device_class()) {
        json.string("device_class", to_string(*value.device_class()));
    }
    if (value.expire_after()) {
        json.integer("expire_after", value.expire_after()->count());
    }
    if (value.force_update()) {
        json.boolean("force_update", *value.force_update());
    }
    if (value.last_reset_value_template()) {
        json.string(
            "last_reset_value_template",
            *value.last_reset_value_template()
        );
    }
    if (!value.options().empty()) {
        json.string_array("options", value.options());
    }
    if (value.suggested_display_precision()) {
        json.integer(
            "suggested_display_precision",
            *value.suggested_display_precision()
        );
    }
    if (value.state_class()) {
        json.string("state_class", to_string(*value.state_class()));
    }
    if (value.unit_of_measurement()) {
        json.string("unit_of_measurement", *value.unit_of_measurement());
    }
    if (value.entity_picture()) {
        json.string("entity_picture", *value.entity_picture());
    }
    if (value.enabled_by_default()) {
        json.boolean("enabled_by_default", *value.enabled_by_default());
    }
    if (value.entity_category()) {
        json.string("entity_category", to_string(*value.entity_category()));
    }
    if (value.icon()) json.string("icon", *value.icon());
    if (value.json_attributes_topic()) {
        json.string("json_attributes_topic", *value.json_attributes_topic());
    }
    if (value.json_attributes_template()) {
        json.string(
            "json_attributes_template",
            *value.json_attributes_template()
        );
    }
    if (value.default_entity_id()) {
        json.string("default_entity_id", *value.default_entity_id());
    }
    if (value.message_expiry_interval()) {
        json.integer(
            "message_expiry_interval",
            *value.message_expiry_interval()
        );
    }
    if (value.visible_by_default()) {
        json.boolean("visible_by_default", *value.visible_by_default());
    }
    if (value.availability_topic()) {
        json.string("availability_topic", *value.availability_topic());
    }
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

} // namespace

std::string serialize_message_payload(
    const Device& device,
    const Origin& origin,
    const std::vector<Component>& components
) {
    JsonObject payload;
    payload.object("device", serialize_device(device));
    payload.object("origin", serialize_origin(origin));
    payload.object("components", serialize_components(components));
    return std::move(payload).finish();
}

} // namespace rhadar::utils
