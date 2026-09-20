#pragma once

#include <optional>
#include <string>
#include <vector>

#include "components/sensor.h"

namespace rhadar {

struct Connection {
    std::string type;       // e.g. "mac"
    std::string identifier; // e.g. "02:5b:26:a8:dc:12"
};

struct Device {
    std::vector<std::string> identifiers; // identifiers or ids
    std::string name; // name

    std::optional<std::string> suggested_area; // suggested_area or sa
    std::optional<std::string> serial_number; // serial_number or sn
    std::optional<std::string> configuration_url; // configuration_url or cu
    std::vector<Connection> connections; // connections or cns

    std::optional<std::string> manufacturer; // manufacturer or mf
    std::optional<std::string> model; // model or mdl
    std::optional<std::string> model_id; // model_id or mdl_id
    std::optional<std::string> sw_version; // sw_version or sw
    std::optional<std::string> hw_version; // hw_version or hw
    std::optional<std::string> via_device; // via_device or hw
};

struct Origin {
    std::string name; // name or name
    std::optional<std::string> sw_version; // sw_version or sw
    std::optional<std::string> support_url; // support_url or url
};

} // namespace rhadar
