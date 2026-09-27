#include "utils/json.h"

#include <charconv>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>

namespace rhadar {

JsonObject::JsonObject(EnumStringFormat key_format)
    : _key_format(key_format) {}

void JsonObject::append_boolean(bool value) {
    _json += value ? "true" : "false";
}

void JsonObject::signed_integer(std::int64_t value) {
    char buffer[20]; // Includes the sign of INT64_MIN.
    const auto result = std::to_chars(buffer, buffer + sizeof(buffer), value);
    _json.append(buffer, result.ptr);
}

void JsonObject::unsigned_integer(std::uint64_t value) {
    char buffer[20]; // UINT64_MAX has 20 decimal digits.
    const auto result = std::to_chars(buffer, buffer + sizeof(buffer), value);
    _json.append(buffer, result.ptr);
}

JsonArray::JsonArray(JsonObject& owner) : _owner(owner) {}

void JsonArray::string(std::string_view value) {
    _owner.value_prefix();
    _owner.quoted(value);
}

void JsonArray::boolean(bool value) {
    _owner.value_prefix();
    _owner.append_boolean(value);
}

std::string JsonObject::finish() && {
    _json += '}';
    return std::move(_json);
}

void JsonObject::value_prefix() {
    if (!_first) _json += ',';
    _first = false;
}

void JsonObject::key_prefix(std::string_view key) {
    value_prefix();
    quoted(key);
    _json += ':';
}

void JsonObject::quoted(std::string_view value) {
    static constexpr char HEX[] = "0123456789abcdef";
    _json += '"';
    for (const unsigned char character : value) {
        switch (character) {
            case '"': _json += "\\\""; break;
            case '\\': _json += "\\\\"; break;
            case '\b': _json += "\\b"; break;
            case '\f': _json += "\\f"; break;
            case '\n': _json += "\\n"; break;
            case '\r': _json += "\\r"; break;
            case '\t': _json += "\\t"; break;
            default:
                if (character < 0x20) {
                    _json += "\\u00";
                    _json += HEX[character >> 4];
                    _json += HEX[character & 0x0f];
                } else {
                    _json += static_cast<char>(character);
                }
        }
    }
    _json += '"';
}

} // namespace rhadar
