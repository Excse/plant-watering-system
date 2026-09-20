#pragma once

#include <optional>
#include <string>

#include "result.h"

namespace rhadar {

// Code Source: https://github.com/home-assistant/core/blob/2026.9.2/homeassistant/components/mqtt/schemas.py#L157
class Origin final {
public:
    [[nodiscard]] const std::string& name() const noexcept { return _name; }

    [[nodiscard]] const std::optional<std::string>& sw_version() const noexcept { return _sw_version; }

    [[nodiscard]] const std::optional<std::string>& support_url() const noexcept { return _support_url; }

private:
    std::string _name; // name or name
    std::optional<std::string> _sw_version; // sw_version or sw
    std::optional<std::string> _support_url; // support_url or url

    friend class OriginBuilder;
};

[[nodiscard]] std::optional<ValidationError> validate(const Origin& origin);

class OriginBuilder {
public:
    explicit OriginBuilder(std::string name);

    [[nodiscard]] OriginBuilder& name(std::string value);

    [[nodiscard]] OriginBuilder& sw_version(std::string value);

    [[nodiscard]] OriginBuilder& support_url(std::string value);

    [[nodiscard]] Result<Origin> build() const;

private:
    Origin _config;
};

} // namespace rhadar
