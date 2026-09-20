#pragma once

#include <optional>
#include <vector>
#include <string>
#include <chrono>

#include "components/base.h"
#include "result.h"

namespace rhadar {

// Code Source: https://github.com/home-assistant/core/blob/2026.9.2/homeassistant/components/sensor/const.py
// Documentation Source: https://www.home-assistant.io/integrations/sensor/#device-class
enum class SensorDeviceClass {
    DATE, // date
    ENUM, // enum
    TIMESTAMP, // timestamp
    UPTIME, // uptime
    ABSOLUTE_HUMIDITY, // absolute_humidity
    APPARENT_POWER, // apparent_power
    AQI, // aqi
    AREA, // area
    ATMOSPHERIC_PRESSURE, // atmospheric_pressure
    BATTERY, // battery
    BLOOD_GLUCOSE_CONCENTRATION, // blood_glucose_concentration
    CO, // carbon_monoxide
    CO2, // carbon_dioxide
    CONDUCTIVITY, // conductivity
    CURRENT, // current
    DATA_RATE, // data_rate
    DATA_SIZE, // data_size
    DISTANCE, // distance
    DURATION, // duration
    ENERGY, // energy
    ENERGY_DISTANCE, // energy_distance
    ENERGY_STORAGE, // energy_storage
    FREQUENCY, // frequency
    GAS, // gas
    HUMIDITY, // humidity
    ILLUMINANCE, // illuminance
    IRRADIANCE, // irradiance
    MOISTURE, // moisture
    MONETARY, // monetary
    NITROGEN_DIOXIDE, // nitrogen_dioxide
    NITROGEN_MONOXIDE, // nitrogen_monoxide
    NITROUS_OXIDE, // nitrous_oxide
    OZONE, // ozone
    PH, // ph
    PM1, // pm1
    PM10, // pm10
    PM25, // pm25
    PM4, // pm4
    POWER_FACTOR, // power_factor
    PRECIPITATION, // precipitation
    PRECIPITATION_INTENSITY, // precipitation_intensity
    PRESSURE, // pressure
    RADON, // radon
    REACTIVE_ENERGY, // reactive_energy
    REACTIVE_POWER, // reactive_power
    SIGNAL_STRENGTH, // signal_strength
    SOUND_PRESSURE, // sound_pressure
    SPEED, // speed
    SULPHUR_DIOXIDE, // sulphur_dioxide
    TEMPERATURE, // temperature
    TEMPERATURE_DELTA, // temperature_delta
    VOLATILE_ORGANIC_COMPOUNDS, // volatile_organic_compounds
    VOLATILE_ORGANIC_COMPOUNDS_PARTS, // volatile_organic_compounds_parts
    VOLTAGE, // voltage
    VOLUME, // volume
    VOLUME_STORAGE, // volume_storage
    VOLUME_FLOW_RATE, // volume_flow_rate
    WATER, // water
    WEIGHT, // weight
    WIND_DIRECTION, // wind_direction
    WIND_SPEED, // wind_speed
};

// Code Source: https://github.com/home-assistant/core/blob/2026.9.2/homeassistant/components/sensor/const.py#L572
// Documentation Source: NaN
enum class SensorStateClass {
    MEASUREMENT, // measurement
    MEASUREMENT_ANGLE, // measurement_angle
    TOTAL, // total
    TOTAL_INCREASING, // total_increasing
};

class SensorBuilder;

// Code Source: https://github.com/home-assistant/core/blob/2026.9.2/homeassistant/components/mqtt/sensor.py#L77
// Documentation Source: NaN
class Sensor final : public Entity {
public:
    [[nodiscard]] const std::string& state_topic() const noexcept { return _state_topic; }

    [[nodiscard]] const std::optional<std::string>& value_template() const noexcept { return _value_template; }

    [[nodiscard]] std::optional<SensorDeviceClass> device_class() const noexcept { return _device_class; }

    [[nodiscard]] std::optional<std::chrono::seconds> expire_after() const noexcept { return _expire_after; }

    [[nodiscard]] std::optional<bool> force_update() const noexcept { return _force_update; }

    [[nodiscard]] const std::optional<std::string>& last_reset_value_template() const noexcept { return _last_reset_value_template; }

    [[nodiscard]] const std::optional<std::string>& name() const noexcept { return _name; }

    [[nodiscard]] const std::vector<std::string>& options() const noexcept { return _options; }

    [[nodiscard]] std::optional<int> suggested_display_precision() const noexcept { return _suggested_display_precision; }

    [[nodiscard]] std::optional<SensorStateClass> state_class() const noexcept { return _state_class; }

    [[nodiscard]] const std::optional<std::string>& unit_of_measurement() const noexcept { return _unit_of_measurement; }

private:
    std::string _state_topic; // Required topic for incoming sensor values.
    std::optional<std::string> _value_template; // value_template or val_tpl
    std::optional<SensorDeviceClass> _device_class; // device_class or dev_cla
    std::optional<std::chrono::seconds> _expire_after; // expire_after or exp_aft
    std::optional<bool> _force_update; // force_update or frc_upd
    std::optional<std::string> _last_reset_value_template; // last_reset_value_template or lrst_val_tpl
    std::optional<std::string> _name; // name or name
    std::vector<std::string> _options; // options or ops
    std::optional<int> _suggested_display_precision; // suggested_display_precision or sug_dsp_prc
    std::optional<SensorStateClass> _state_class; // state_class or stat_cla
    std::optional<std::string> _unit_of_measurement; // unit_of_measurement or unit_of_meas

    friend class SensorBuilder;
    friend class EntityBuilder<SensorBuilder, Sensor>;
};

[[nodiscard]] std::optional<ValidationError> validate(const Sensor& sensor);

class SensorBuilder : public EntityBuilder<SensorBuilder, Sensor> {
public:
    explicit SensorBuilder(std::string unique_id);

    [[nodiscard]] SensorBuilder& state_topic(std::string value);

    [[nodiscard]] SensorBuilder& value_template(std::string value);

    [[nodiscard]] SensorBuilder& name(std::string value);

    [[nodiscard]] SensorBuilder& device_class(SensorDeviceClass value);

    [[nodiscard]] SensorBuilder& state_class(SensorStateClass value);

    [[nodiscard]] SensorBuilder& unit_of_measurement(std::string value);

    [[nodiscard]] SensorBuilder& expire_after(std::chrono::seconds value);

    [[nodiscard]] SensorBuilder& suggested_display_precision(int value);

    [[nodiscard]] SensorBuilder& force_update(bool value);

    [[nodiscard]] SensorBuilder& last_reset_value_template(std::string value);

    [[nodiscard]] SensorBuilder& options(std::vector<std::string> values);

    [[nodiscard]] SensorBuilder& add_option(std::string value);

    [[nodiscard]] Result<Sensor> build() const;
};

extern template class EntityBuilder<SensorBuilder, Sensor>;

} // namespace rhadar
