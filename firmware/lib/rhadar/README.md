# rhadar

**rhadar** — Radar for Home Assistant.

A generic C++ library for Home Assistant MQTT discovery, intended for reuse
across projects, boards, and frameworks.
The API requires C++23 or later and includes a sensor configuration builder with
validation using `std::expected`.
Discovery serialization and MQTT publishing are not implemented yet.

## Documentation

See the [Home Assistant MQTT Discovery specification guide](MQTT_DISCOVERY.md)
for protocol rules, all supported components, configuration fields, runtime
payloads, topic and abbreviation indexes, and lifecycle behavior.

## Structure

- `include/rhadar.h`: public C++ API.
- `include/components/base.h`: entity configuration, validation and shared builder.
- `include/components/sensor.h`: sensor configuration, builder and validation.
- `include/result.h`: validation errors and the `std::expected` result alias.
- `src/components/sensor.cpp`: sensor builder and validation definitions.
- `src/components/base.cpp`: entity validation, shared setters and builder instantiations.
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
    .device_class(rhadar::SensorDeviceClass::MOISTURE)
    .state_class(rhadar::SensorStateClass::MEASUREMENT)
    .unit_of_measurement("%")
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

`Entity` and `Sensor` expose read-only accessors and keep all configuration fields
private. Use `SensorBuilder` for validated configurations; getters such as `name()` and
`options()` return const references, while small optional values are returned by
value. References remain valid while the owning sensor exists and is not replaced
or moved from. Fields cannot be assigned directly. Default construction is available
through the compiler-generated constructor; it does not perform validation.

Each successful build returns an independent snapshot. You can copy, move, or
replace an entire `Sensor`. Construction, copying, moving, and assignment use
compiler-generated special members. The builder remains reusable and subsequent changes do not affect
previous results.

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
templates, and the full device-class/unit compatibility table are not checked.
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

Error enum values
use `UPPER_SNAKE_CASE`, for example `ValidationErrorCode::MISSING_STATE_TOPIC`.
