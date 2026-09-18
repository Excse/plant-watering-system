# rhadar

**rhadar** — Radar for Home Assistant.

A generic C++ library for Home Assistant MQTT discovery, intended for reuse
across projects, boards, and frameworks.
This is an initial scaffold; discovery functionality is not implemented yet.

## Documentation

See the [Home Assistant MQTT Discovery specification guide](MQTT_DISCOVERY.md)
for protocol rules, all supported components, configuration fields, runtime
payloads, topic and abbreviation indexes, and lifecycle behavior.

## Structure

- `include/rhadar.h`: public C++ API.
- `src/rhadar.cpp`: implementation.
- `library.json`: PlatformIO library metadata.

## Usage

Include the public header from C++ code:

```cpp
#include <rhadar.h>
```

PlatformIO discovers this library under `lib/` when project code includes its
header. Add public declarations to the `rhadar` namespace and their
implementations to `src/`.

For use outside PlatformIO, add `src/rhadar.cpp` to your build and `include/`
to your compiler's header search paths. Calling the C++ API from C requires
a C-compatible wrapper.
