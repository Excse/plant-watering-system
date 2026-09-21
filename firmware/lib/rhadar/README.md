# rhadar

**rhadar** — Radar for Home Assistant.

A generic C++ library for Home Assistant MQTT discovery, intended for reuse
across projects, boards, and frameworks.
The API requires C++23 or later and includes sensor, device, connection, and origin
configuration builders with validation using `std::expected`.
Discovery serialization and MQTT publishing are not implemented yet.

## Documentation

See the [Home Assistant MQTT Discovery specification guide](MQTT_DISCOVERY.md)
for protocol rules, all supported components, configuration fields, runtime
payloads, topic and abbreviation indexes, and lifecycle behavior.

## Structure

- `include/rhadar.h`: public C++ API.
- `include/components/base.h`: entity configuration, validation and shared builder.
- `include/components/sensor.h`: sensor configuration, builder and validation.
- `include/message.h`: transport-independent MQTT discovery messages.
- `include/connection.h`, `include/device.h`, `include/origin.h`: read-only
  configuration classes, builders, and validation declarations.
- `include/result.h`: validation errors and the `std::expected` result alias.
- `include/utils/enum.h`: shared enum declaration and string conversion macros.
- `src/components/sensor.cpp`: sensor builder and validation definitions.
- `src/components/base.cpp`: entity validation, shared setters and builder instantiations.
- `src/message.cpp`: message construction and discovery-topic validation.
- `src/utils/json.cpp`: JSON encoding and message-payload serialization.
- `src/connection.cpp`, `src/device.cpp`, `src/origin.cpp`: corresponding builder
  constructors, setters, and validators.
- `library.json`: PlatformIO library metadata.

## Usage

Include the public header:

```cpp
#include <rhadar.h>
```

Inside your application function, build a sensor configuration:

```cpp
auto result = rhadar::SensorBuilder{"pws_abc123_moisture"}
    .icon("mdi:water-percent")
    .state_topic("pws_abc123/moisture/state")
    .name("Soil moisture")
    .device_class(rhadar::SensorDeviceClass::Moisture)
    .state_class(rhadar::SensorStateClass::Measurement)
    .unit_of_measurement("%")
    .availability_topic("pws_abc123/status")
    .expire_after(std::chrono::seconds{300})
    .suggested_display_precision(0)
    .enabled_by_default(true)
    .build();

if (!result) {
    // Handle result.error().code, .field and .message.
    return;
}

const rhadar::Sensor& sensor = *result;
// Use sensor while result remains alive, or copy it for independent ownership.
// Read fields with sensor.state_topic(), sensor.name(), sensor.unique_id(), etc.
```

`SensorBuilder` requires an identity argument. Its setters own their string inputs
and return `SensorBuilder&`, including inherited entity settings. Setters replace
values; `add_option()` appends. `options({})` clears the list, meaning it is omitted.
Unspecified optional fields remain unset; explicitly supplied `false` and `0` are
preserved.

`Entity`, `Sensor`, `Connection`, `Device`, and `Origin` expose read-only accessors
and keep all configuration fields private. Use their builders for validated
configurations; getters such as `name()` and
`options()` return const references, while small optional values are returned by
value. References remain valid while the owning sensor exists and is not replaced
or moved from. Fields cannot be assigned directly. Default construction is available
through the compiler-generated constructor; it does not perform validation.

Each successful build returns an independent snapshot. You can copy, move, or
replace an entire `Sensor`. Construction, copying, moving, and assignment use
compiler-generated special members. The builder remains reusable and subsequent changes do not affect
previous results.

Device and publisher metadata use the same pattern:

```cpp
auto connection = rhadar::ConnectionBuilder{"mac", "02:5b:26:a8:dc:12"}.build();
if (!connection) {
    // Handle connection.error().
    return;
}

auto device = rhadar::DeviceBuilder{"pws_abc123"}
    .name("Plant watering system")
    .manufacturer("DIY")
    .model("ESP32")
    .add_connection(*connection)
    .build();
if (!device) {
    // Handle device.error().
    return;
}

auto origin = rhadar::OriginBuilder{"plant-watering-system"}
    .sw_version("1.0.0")
    .build();
if (!origin) {
    // Handle origin.error().
    return;
}

// Read device->name(), device->connections(), origin->sw_version(), etc.
```

`ConnectionBuilder` takes a type and identifier; both must be nonempty. Connection
types remain strings so additional connection types can be used without library
changes. `OriginBuilder` takes the publisher name, which must be nonempty.

`DeviceBuilder` starts with an identifier or a `Connection`. `identifiers()` and
`connections()` replace the respective collections; `add_identifier()` and
`add_connection()` append. Its validator requires at least one identifier or
connection, rejects empty identifier entries, and validates every nested
connection. Nested errors identify the entry, for example
`connections[0].identifier`. Each class has its own `validate(const T&)` overload,
and `build()` returns `Result<T>` without consuming the builder.

Compose those snapshots into a transport-independent device-discovery message.
`Device` and `Origin` are constructor arguments because Home Assistant requires
both at the root of a device-discovery envelope:

```cpp
auto message = rhadar::MessageBuilder{
        *device,
        *origin,
        "pws_abc123"
    }
    .add_component("moisture", *sensor_result)
    .qos(1)
    .build();

if (!message) {
    // Handle message.error().
    return;
}

esp_mqtt_client_publish(
    client,
    message->topic().c_str(),
    message->payload().c_str(),
    static_cast<int>(message->payload().size()),
    message->qos(),
    message->retain()
);
```

With the values above, the topic is
`homeassistant/device/pws_abc123/config`. Calling `node_id("greenhouse")` produces
`homeassistant/device/greenhouse/pws_abc123/config`. `discovery_prefix()` overrides
the default `homeassistant` prefix. Node, object, and component IDs accept letters,
digits, `_`, and `-`.

Add components individually, or replace the complete collection:

```cpp
std::vector<rhadar::DiscoveryComponent> components{
    {"moisture", *moisture_sensor},
    {"temperature", *temperature_sensor},
};

auto message = rhadar::MessageBuilder{*device, *origin, "pws_abc123"}
    .components(std::move(components))
    .build();
```

`components()` replaces the vector, while `add_component()` appends. Component
IDs must be unique, and at least one component is required. Each component owns
an immutable `ComponentEntity`, which is a `std::variant` of the supported entity
configuration types. Validation, platform selection, and serialization use
`std::visit`, making dispatch exhaustive at compile time without virtual methods
or per-component heap allocation.

The variant currently contains `Sensor`, the component type implemented by the
library today. When `Light` is added, it should become
`std::variant<Sensor, Light>` with corresponding validation and serialization
overloads. `MessageBuilder` and its component vector do not otherwise
change, and the serialized light component will select `"platform":"light"`.

The message owns its topic and compact JSON payload, so their `c_str()` pointers
remain valid while the message exists. Discovery messages are always retained;
QoS defaults to 0 and may be set to 0, 1, or 2. The builder validates its required
device and origin and every component again before serializing. It escapes JSON
strings and omits unset optional fields while preserving explicitly configured
`false` and `0` values. MQTT connection management and publication remain the
application's responsibility.

`SensorBuilder` inherits common settings from `EntityBuilder<SensorBuilder, Sensor>`.
The base owns the complete sensor configuration and initializes its entity identity.
Shared setters return `SensorBuilder&`, so entity and sensor options can be mixed
in a single chain. Sensor-specific setters and `build()` use that same configuration.

`build()` validates and returns an independent snapshot in `Result<Sensor>`
(an alias for `std::expected<Sensor, ValidationError>`);
the builder remains reusable, including after an unsuccessful build. It checks:

- Nonempty identity (a library requirement) and state topic.
- Nonnegative expiration, display precision and message expiry interval.
- Diagnostic-only sensor entity categories when specified.
- Options requiring the enum device class and no state class or unit.
- Last-reset templates requiring the total state class.
- Angle measurements requiring degree units (`°`).

`validate(const Entity&)` checks the library's required nonempty identity,
nonnegative message expiry interval, and a valid category (`CONFIG` or `DIAGNOSTIC`
when set). It returns `std::nullopt` on success or the first `ValidationError`.

`validate(const Sensor&)` first calls the entity validator, then checks the
sensor-specific rules, including the restriction to the diagnostic category.
`SensorBuilder::build()` calls this combined validator. Both overloads also accept
existing snapshots and read their configuration through the public accessors.

These checks are not full Home Assistant schema validation: topic syntax,
templates, metadata URLs, and the full device-class/unit compatibility table are
not checked.
JSON null values are not modeled separately from omitted optional values yet.
Cross-field rules follow the
[Home Assistant MQTT sensor schema](https://github.com/home-assistant/core/blob/2026.9.2/homeassistant/components/mqtt/sensor.py).

Check the result before dereferencing it or accessing its error; accessing the
wrong alternative violates the standard type's preconditions. Validation failures
are returned with `std::unexpected` and do not throw exceptions.
The API works with exceptions and RTTI disabled, but uses standard strings and
vectors, which allocate memory.

PlatformIO discovers this library under `lib/` when project code includes its
header.

For use outside PlatformIO, compile all `.cpp` files under `src/` (including
subdirectories), enable C++23 or later, and add `include/` to your compiler's header
search paths. Calling the C++ API from C requires a C-compatible wrapper.

Simple getters are defined inline in the headers. Builder constructors, setters,
`build()`, and validators are defined in `.cpp` files. Default constructors and
other special members are left to the compiler.
The standard library supplies the result implementation; no custom template
definitions or explicit instantiations are needed for `Result`.
The shared `EntityBuilder` template is explicitly instantiated in
`src/components/base.cpp`. When adding another component builder, include its
header there and add `template class EntityBuilder<NewBuilder, NewConfig>;` after
the template definitions. Add the matching `extern template` declaration after
the new builder's class declaration in its header.

Enum values use `PascalCase`, for example
`ValidationErrorCode::MissingStateTopic`.
All library enums use an X-macro value table. `DEFINE_ENUM` accepts one wire
name per value. `DEFINE_ABBREVIATED_ENUM` accepts distinct full and abbreviated
wire names. Both provide the enum declaration and the shared
`rhadar::to_string()` conversion without a separate enum-specific switch. Pass
`EnumStringFormat::Abbreviated` to select the abbreviated form.

## Test

The repository contains one host-side construction test for a complete device
discovery message. Run it from the firmware directory:

```sh
c++ -std=c++23 -Wall -Wextra -Werror -pedantic -fno-exceptions -fno-rtti \
    -Ilib/rhadar/include \
    lib/rhadar/src/*.cpp lib/rhadar/src/components/*.cpp \
    lib/rhadar/src/utils/*.cpp \
    test/rhadar_message.cpp -o /tmp/rhadar-message-test
/tmp/rhadar-message-test
```
