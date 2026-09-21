#pragma once

#include <string_view>

namespace rhadar {

enum class EnumStringFormat {
    Full,
    Abbreviated,
};

template <typename E>
struct enum_traits;

template <typename E>
[[nodiscard]] constexpr std::string_view to_string(E value,EnumStringFormat format = EnumStringFormat::Full) {
    return enum_traits<E>::to_string(value, format);
}

} // namespace rhadar

#define ENUM_VALUE(TargetEnum, name, full) \
    name,

#define ENUM_CASE(TargetEnum, name, full) \
    case TargetEnum::name: return full;

#define ABBREVIATED_ENUM_VALUE(TargetEnum, name, full, abbreviation) \
    name,

#define ABBREVIATED_ENUM_CASE(TargetEnum, name, full, abbreviation) \
    case TargetEnum::name:                                          \
        return format == rhadar::EnumStringFormat::Abbreviated      \
            ? abbreviation                                          \
            : full;

#define DEFINE_ENUM(TargetEnum, VALUES)                                                                 \
    enum class TargetEnum {                                                                             \
        VALUES(ENUM_VALUE, TargetEnum)                                                                  \
    };                                                                                                  \
                                                                                                        \
    template <>                                                                                         \
    struct enum_traits<TargetEnum> {                                                                    \
        static constexpr std::string_view to_string(TargetEnum value, rhadar::EnumStringFormat) {       \
            switch (value) {                                                                            \
                VALUES(ENUM_CASE, TargetEnum)                                                           \
            }                                                                                           \
            return "Unknown";                                                                           \
        }                                                                                               \
    };

#define DEFINE_ABBREVIATED_ENUM(TargetEnum, VALUES)                                                         \
    enum class TargetEnum {                                                                                 \
        VALUES(ABBREVIATED_ENUM_VALUE, TargetEnum)                                                          \
    };                                                                                                      \
                                                                                                            \
    template <>                                                                                             \
    struct enum_traits<TargetEnum> {                                                                        \
        static constexpr std::string_view to_string(TargetEnum value, rhadar::EnumStringFormat format) {    \
            switch (value) {                                                                                \
                VALUES(ABBREVIATED_ENUM_CASE, TargetEnum)                                                   \
            }                                                                                               \
            return "Unknown";                                                                               \
        }                                                                                                   \
    };
