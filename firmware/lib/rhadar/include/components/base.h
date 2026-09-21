#pragma once

#include <optional>
#include <vector>
#include <string>

#include "utils/enum.h"
#include "result.h"

namespace rhadar {

// Code Source: https://github.com/home-assistant/core/blob/2026.9.2/homeassistant/const.py#L1026
// Documentation Source: NaN
#define ENTITY_CATEGORIES(X, TargetEnum)    \
    X(TargetEnum, Config,     "config")     \
    X(TargetEnum, Diagnostic, "diagnostic")
DEFINE_ENUM(EntityCategory, ENTITY_CATEGORIES)

// Code Source: https://github.com/home-assistant/core/blob/2026.9.2/homeassistant/components/mqtt/schemas.py#L175
// Documentation Source: NaN
class Entity {
public:
    [[nodiscard]] const std::optional<std::string>& entity_picture() const noexcept { return _entity_picture; }

    [[nodiscard]] std::optional<bool> enabled_by_default() const noexcept { return _enabled_by_default; }

    [[nodiscard]] std::optional<EntityCategory> entity_category() const noexcept { return _entity_category; }

    [[nodiscard]] const std::optional<std::string>& icon() const noexcept { return _icon; }

    [[nodiscard]] const std::optional<std::string>& json_attributes_topic() const noexcept { return _json_attributes_topic; }

    [[nodiscard]] const std::optional<std::string>& json_attributes_template() const noexcept { return _json_attributes_template; }

    [[nodiscard]] const std::optional<std::string>& default_entity_id() const noexcept { return _default_entity_id; }

    [[nodiscard]] std::optional<int> message_expiry_interval() const noexcept { return _message_expiry_interval; }

    [[nodiscard]] const std::optional<std::string>& unique_id() const noexcept { return _unique_id; }
    
    [[nodiscard]] std::optional<bool> visible_by_default() const noexcept { return _visible_by_default; }

    [[nodiscard]] const std::optional<std::string>& availability_topic() const noexcept { return _availability_topic; }

private:
    std::optional<std::string> _entity_picture; // entity_picture or ent_pic
    std::optional<bool> _enabled_by_default; // enabled_by_default or en
    std::optional<EntityCategory> _entity_category; // entity_category or ent_cat
    std::optional<std::string> _icon; // icon or ic
    std::optional<std::string> _json_attributes_topic; // json_attributes_topic or json_attr_t
    std::optional<std::string> _json_attributes_template; // json_attributes_template or json_attr_tpl
    std::optional<std::string> _default_entity_id; // default_entity_id or def_ent_id
    std::optional<int> _message_expiry_interval; // message_expiry_interval or msg_exp_int
    std::optional<std::string> _unique_id; // unique_id or uniq_id
    std::optional<bool> _visible_by_default; // visible_by_default or vis
    std::optional<std::string> _availability_topic; // availability_topic or avty_t

    template <typename Derived, typename Config>
    friend class EntityBuilder;
};

[[nodiscard]] std::optional<ValidationError> validate(const Entity& entity);

template <typename Derived, typename Config>
class EntityBuilder {
public:
    [[nodiscard]] Derived& entity_picture(std::string value);

    [[nodiscard]] Derived& enabled_by_default(bool value);

    [[nodiscard]] Derived& entity_category(EntityCategory value);

    [[nodiscard]] Derived& icon(std::string value);

    [[nodiscard]] Derived& json_attributes_topic(std::string value);

    [[nodiscard]] Derived& json_attributes_template(std::string value);

    [[nodiscard]] Derived& default_entity_id(std::string value);

    [[nodiscard]] Derived& message_expiry_interval(int value);

    [[nodiscard]] Derived& visible_by_default(bool value);

    [[nodiscard]] Derived& availability_topic(std::string value);

protected:
    explicit EntityBuilder(std::string unique_id);

    Config _config;
};

} // namespace rhadar
