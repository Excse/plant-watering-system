#include "utils/serializer.h"

#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "message.h"
#include "utils/json.h"

namespace rhadar {

void serialize_device(JsonObject& json, const Device& value) {
    json.array(DeviceFields::Identifiers, [&](JsonArray& identifiers) {
        for (const auto& identifier : value.identifiers()) {
            identifiers.string(identifier);
        }
    });
    json.string(DeviceFields::Name, value.name());
    json.string(DeviceFields::SuggestedArea, value.suggested_area());
    json.string(DeviceFields::SerialNumber, value.serial_number());
    json.string(DeviceFields::ConfigurationUrl, value.configuration_url());
    json.array(DeviceFields::Connections, [&](JsonArray& connections) {
        for (const auto& connection : value.connections()) {
            connections.array([&](JsonArray& pair) {
                pair.string(connection.type());
                pair.string(connection.identifier());
            });
        }
    });
    json.string(DeviceFields::Manufacturer, value.manufacturer());
    json.string(DeviceFields::Model, value.model());
    json.string(DeviceFields::ModelId, value.model_id());
    json.string(DeviceFields::SwVersion, value.sw_version());
    json.string(DeviceFields::HwVersion, value.hw_version());
}

void serialize_origin(JsonObject& json, const Origin& value) {
    json.string(OriginFields::Name, value.name());
    json.string(OriginFields::SwVersion, value.sw_version());
    json.string(OriginFields::SupportUrl, value.support_url());
}

void serialize_component(JsonObject& json, const Sensor& value) {
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
}

void serialize_components(JsonObject& json, const std::vector<Component>& values) {
    for (const auto& value : values) {
        json.object(value.id(), [&](JsonObject& component) {
            std::visit(
                [&](const auto& value) { 
                    serialize_component(component, value); 
                },
                value.entity()
            );
        });
    }
}

std::string serialize_message_payload(
    const Device& device,
    const Origin& origin,
    const std::vector<Component>& components
) {
    JsonObject payload;
    
    payload.object(MessageFields::Device, [&](JsonObject& json) {
        serialize_device(json, device);
    });

    payload.object(MessageFields::Origin, [&](JsonObject& json) {
        serialize_origin(json, origin);
    });

    payload.object(MessageFields::Components, [&](JsonObject& json) {
        serialize_components(json, components);
    });

    return std::move(payload).finish();
}

} // namespace rhadar
