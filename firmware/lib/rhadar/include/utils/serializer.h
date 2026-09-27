#pragma once

#include <string>
#include <vector>

namespace rhadar {

class Component;
class Device;
class Origin;

[[nodiscard]] std::string serialize_message_payload(
    const Device& device,
    const Origin& origin,
    const std::vector<Component>& components
);

} // namespace rhadar
