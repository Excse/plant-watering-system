#pragma once

#include <optional>
#include <vector>
#include <string>
#include <chrono>

#include "components/base.h"
#include "result.h"

namespace rhadar {

class SensorBuilder;

// Code Source: https://github.com/home-assistant/core/blob/2026.9.2/homeassistant/components/sensor/const.py
// Documentation Source: https://www.home-assistant.io/integrations/sensor/#device-class
#define SENSOR_DEVICE_CLASSES(X, TargetEnum)                                            \
    X(TargetEnum, Date,                          "date")                                \
    X(TargetEnum, Enum,                          "enum")                                \
    X(TargetEnum, Timestamp,                     "timestamp")                           \
    X(TargetEnum, Uptime,                        "uptime")                              \
    X(TargetEnum, AbsoluteHumidity,              "absolute_humidity")                   \
    X(TargetEnum, ApparentPower,                 "apparent_power")                      \
    X(TargetEnum, Aqi,                           "aqi")                                 \
    X(TargetEnum, Area,                          "area")                                \
    X(TargetEnum, AtmosphericPressure,           "atmospheric_pressure")                \
    X(TargetEnum, Battery,                       "battery")                             \
    X(TargetEnum, BloodGlucoseConcentration,     "blood_glucose_concentration")         \
    X(TargetEnum, Co,                            "carbon_monoxide")                     \
    X(TargetEnum, Co2,                           "carbon_dioxide")                      \
    X(TargetEnum, Conductivity,                  "conductivity")                        \
    X(TargetEnum, Current,                       "current")                             \
    X(TargetEnum, DataRate,                      "data_rate")                           \
    X(TargetEnum, DataSize,                      "data_size")                           \
    X(TargetEnum, Distance,                      "distance")                            \
    X(TargetEnum, Duration,                      "duration")                            \
    X(TargetEnum, Energy,                        "energy")                              \
    X(TargetEnum, EnergyDistance,                "energy_distance")                     \
    X(TargetEnum, EnergyStorage,                 "energy_storage")                      \
    X(TargetEnum, Frequency,                     "frequency")                           \
    X(TargetEnum, Gas,                           "gas")                                 \
    X(TargetEnum, Humidity,                      "humidity")                            \
    X(TargetEnum, Illuminance,                   "illuminance")                         \
    X(TargetEnum, Irradiance,                    "irradiance")                          \
    X(TargetEnum, Moisture,                      "moisture")                            \
    X(TargetEnum, Monetary,                      "monetary")                            \
    X(TargetEnum, NitrogenDioxide,               "nitrogen_dioxide")                    \
    X(TargetEnum, NitrogenMonoxide,              "nitrogen_monoxide")                   \
    X(TargetEnum, NitrousOxide,                  "nitrous_oxide")                       \
    X(TargetEnum, Ozone,                         "ozone")                               \
    X(TargetEnum, Ph,                            "ph")                                  \
    X(TargetEnum, Pm1,                           "pm1")                                 \
    X(TargetEnum, Pm10,                          "pm10")                                \
    X(TargetEnum, Pm25,                          "pm25")                                \
    X(TargetEnum, Pm4,                           "pm4")                                 \
    X(TargetEnum, PowerFactor,                   "power_factor")                        \
    X(TargetEnum, Precipitation,                 "precipitation")                       \
    X(TargetEnum, PrecipitationIntensity,        "precipitation_intensity")             \
    X(TargetEnum, Pressure,                      "pressure")                            \
    X(TargetEnum, Radon,                         "radon")                               \
    X(TargetEnum, ReactiveEnergy,                "reactive_energy")                     \
    X(TargetEnum, ReactivePower,                 "reactive_power")                      \
    X(TargetEnum, SignalStrength,                "signal_strength")                     \
    X(TargetEnum, SoundPressure,                 "sound_pressure")                      \
    X(TargetEnum, Speed,                         "speed")                               \
    X(TargetEnum, SulphurDioxide,                "sulphur_dioxide")                     \
    X(TargetEnum, Temperature,                   "temperature")                         \
    X(TargetEnum, TemperatureDelta,              "temperature_delta")                   \
    X(TargetEnum, VolatileOrganicCompounds,      "volatile_organic_compounds")          \
    X(TargetEnum, VolatileOrganicCompoundsParts, "volatile_organic_compounds_parts")    \
    X(TargetEnum, Voltage,                       "voltage")                             \
    X(TargetEnum, Volume,                        "volume")                              \
    X(TargetEnum, VolumeStorage,                 "volume_storage")                      \
    X(TargetEnum, VolumeFlowRate,                "volume_flow_rate")                    \
    X(TargetEnum, Water,                         "water")                               \
    X(TargetEnum, Weight,                        "weight")                              \
    X(TargetEnum, WindDirection,                 "wind_direction")                      \
    X(TargetEnum, WindSpeed,                     "wind_speed")
DEFINE_ENUM(SensorDeviceClass, SENSOR_DEVICE_CLASSES)

// Code Source: https://github.com/home-assistant/core/blob/2026.9.2/homeassistant/components/sensor/const.py#L572
// Documentation Source: NaN
#define SENSOR_STATE_CLASSES(X, TargetEnum)                 \
    X(TargetEnum, Measurement,      "measurement")          \
    X(TargetEnum, MeasurementAngle, "measurement_angle")    \
    X(TargetEnum, Total,            "total")                \
    X(TargetEnum, TotalIncreasing,  "total_increasing")
DEFINE_ENUM(SensorStateClass, SENSOR_STATE_CLASSES)

// Code Source: https://github.com/home-assistant/core/blob/2026.9.2/homeassistant/components/mqtt/sensor.py#L77
// Documentation Source: NaN
#define SENSOR_FIELDS(X, TargetEnum)                                                    \
    X(TargetEnum, StateTopic,                "state_topic",                 "stat_t")  \
    X(TargetEnum, ValueTemplate,             "value_template",              "val_tpl") \
    X(TargetEnum, DeviceClass,               "device_class",                "dev_cla") \
    X(TargetEnum, ExpireAfter,               "expire_after",                "exp_aft") \
    X(TargetEnum, ForceUpdate,               "force_update",                "frc_upd") \
    X(TargetEnum, LastResetValueTemplate,    "last_reset_value_template",   "lrst_val_tpl") \
    X(TargetEnum, Name,                      "name",                        "name") \
    X(TargetEnum, Options,                   "options",                     "ops") \
    X(TargetEnum, SuggestedDisplayPrecision, "suggested_display_precision", "sug_dsp_prc") \
    X(TargetEnum, StateClass,                "state_class",                 "stat_cla") \
    X(TargetEnum, UnitOfMeasurement,         "unit_of_measurement",         "unit_of_meas")
DEFINE_ABBREVIATED_ENUM(SensorFields, SENSOR_FIELDS)

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
    std::string _state_topic;
    std::optional<std::string> _value_template;
    std::optional<SensorDeviceClass> _device_class;
    std::optional<std::chrono::seconds> _expire_after;
    std::optional<bool> _force_update;
    std::optional<std::string> _last_reset_value_template;
    std::optional<std::string> _name;
    std::vector<std::string> _options;
    std::optional<int> _suggested_display_precision;
    std::optional<SensorStateClass> _state_class;
    std::optional<std::string> _unit_of_measurement;

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
