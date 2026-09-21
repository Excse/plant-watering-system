#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "components/sensor.h"
#include "connection.h"
#include "message.h"
#include "device.h"
#include "origin.h"

void require(bool condition, const char* message) {
    if (condition) return;
    std::cerr << message << '\n';
    std::abort();
}

void constructs_entire_message() {
    auto device = rhadar::DeviceBuilder("pws_abc123")
        .name("Plant watering system")
        .manufacturer("DIY")
        .model("ESP32")
        .add_connection("mac", "02:5b:26:a8:dc:12")
        .build();
    require(device.has_value(), "device construction failed");

    auto origin = rhadar::OriginBuilder("plant-watering-system")
        .sw_version("1.0.0")
        .build();
    require(origin.has_value(), "origin construction failed");

    auto moisture = rhadar::SensorBuilder("pws_abc123_moisture")
        .state_topic("pws_abc123/moisture/state")
        .name("Soil moisture")
        .device_class(rhadar::SensorDeviceClass::Moisture)
        .state_class(rhadar::SensorStateClass::Measurement)
        .unit_of_measurement("%")
        .availability_topic("pws_abc123/status")
        .force_update(false)
        .suggested_display_precision(0)
        .build();
    require(moisture.has_value(), "moisture sensor construction failed");

    auto temperature = rhadar::SensorBuilder("pws_abc123_temperature")
        .state_topic("pws_abc123/temperature/state")
        .name("Temperature")
        .device_class(rhadar::SensorDeviceClass::Temperature)
        .state_class(rhadar::SensorStateClass::Measurement)
        .unit_of_measurement("C")
        .build();
    require(temperature.has_value(), "temperature sensor construction failed");

    std::vector<rhadar::Component> components{
        {"moisture", *moisture},
        {"temperature", *temperature},
    };

    auto message = rhadar::MessageBuilder(*device, *origin, "pws_abc123")
        .node_id("greenhouse")
        .components(std::move(components))
        .qos(1)
        .build();
    require(message.has_value(), "message construction failed");

    require(message->topic() == "homeassistant/device/greenhouse/pws_abc123/config", "unexpected discovery topic");
    require(message->qos() == 1, "unexpected discovery QoS");
    require(message->retain(), "message must be retained");

    const std::string expected_payload =
        "{\"device\":{\"identifiers\":[\"pws_abc123\"],"
        "\"name\":\"Plant watering system\","
        "\"connections\":[[\"mac\",\"02:5b:26:a8:dc:12\"]],"
        "\"manufacturer\":\"DIY\",\"model\":\"ESP32\"},"
        "\"origin\":{\"name\":\"plant-watering-system\","
        "\"sw_version\":\"1.0.0\"},"
        "\"components\":{"
        "\"moisture\":{\"platform\":\"sensor\","
        "\"unique_id\":\"pws_abc123_moisture\","
        "\"state_topic\":\"pws_abc123/moisture/state\","
        "\"name\":\"Soil moisture\",\"device_class\":\"moisture\","
        "\"force_update\":false,\"suggested_display_precision\":0,"
        "\"state_class\":\"measurement\",\"unit_of_measurement\":\"%\","
        "\"availability_topic\":\"pws_abc123/status\"},"
        "\"temperature\":{\"platform\":\"sensor\","
        "\"unique_id\":\"pws_abc123_temperature\","
        "\"state_topic\":\"pws_abc123/temperature/state\","
        "\"name\":\"Temperature\",\"device_class\":\"temperature\","
        "\"state_class\":\"measurement\",\"unit_of_measurement\":\"C\"}}}";

    require(message->payload() == expected_payload, "unexpected discovery payload");
}

int main() {
    constructs_entire_message();
    return 0;
}
