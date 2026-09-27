#pragma once

#include <chrono>
#include <concepts>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "utils/enum.h"

namespace rhadar {

template <typename T>
concept JsonInteger = 
    std::integral<T> &&
    !std::same_as<std::remove_cv_t<T>, bool> && 
    sizeof(T) <= sizeof(std::uint64_t);

class JsonArray;

class JsonObject {
public:
    explicit JsonObject(EnumStringFormat key_format = EnumStringFormat::Full);

    JsonObject(const JsonObject&) = delete;

    JsonObject& operator=(const JsonObject&) = delete;

    template <typename Key>
    void string(const Key& key, std::string_view value) {
        key_prefix(key);
        quoted(value);
    }

    template <typename Key, typename T>
        requires (std::is_same_v<T, std::string> || std::is_enum_v<T>)
    void string(const Key& key, const std::optional<T>& value) {
        if (!value) return;

        if constexpr (std::is_enum_v<T>) {
            string(key, to_string(*value));
        } else {
            string(key, *value);
        }
    }

    template <typename Key>
    void boolean(const Key& key, bool value) {
        key_prefix(key);
        append_boolean(value);
    }

    template <typename Key>
    void boolean(const Key& key, const std::optional<bool>& value) {
        if (!value) return;
        
        boolean(key, *value);
    }

    template <typename Key, JsonInteger T>
    void integer(const Key& key, T value) {
        key_prefix(key);
        append_integer(value);
    }

    template <typename Key, JsonInteger T>
    void integer(const Key& key, const std::optional<T>& value) {
        if (!value) return;
        
        integer(key, *value);
    }

    template <typename Key, JsonInteger Rep, typename Period>
    void integer(const Key& key, std::chrono::duration<Rep, Period> value) {
        integer(key, value.count());
    }

    template <typename Key, JsonInteger Rep, typename Period>
    void integer(const Key& key, const std::optional<std::chrono::duration<Rep, Period>>& value) {
        if (value) return;
        
        integer(key, *value);
    }

    template <typename Key, typename F>
        requires std::invocable<F, JsonObject&>
    void object(const Key& key, F&& write) {
        key_prefix(key);
        write_object(std::forward<F>(write));
    }

    template <typename Key, typename F>
        requires std::invocable<F, JsonArray&>
    void array(const Key& key, F&& write) {
        key_prefix(key);
        write_array(std::forward<F>(write));
    }

    template <typename Key>
    void string_array(const Key& key, const std::vector<std::string>& values);

    [[nodiscard]] std::string finish() &&;

private:
    friend class JsonArray;

    void value_prefix();
    void key_prefix(std::string_view key);

    template <typename E>
        requires std::is_enum_v<E>
    void key_prefix(E key) {
        key_prefix(to_string(key, _key_format));
    }

    void quoted(std::string_view value);
    void append_boolean(bool value);
    void signed_integer(std::int64_t value);
    void unsigned_integer(std::uint64_t value);

    template <JsonInteger T>
    void append_integer(T value) {
        if constexpr (std::is_signed_v<T>) {
            signed_integer(static_cast<std::int64_t>(value));
        } else {
            unsigned_integer(static_cast<std::uint64_t>(value));
        }
    }

    template <typename F>
    void write_object(F&& write) {
        _json += '{';
        const bool first = std::exchange(_first, true);
        std::forward<F>(write)(*this);
        _json += '}';
        _first = first;
    }

    template <typename F>
    void write_array(F&& write) {
        _json += '[';
        const bool first = std::exchange(_first, true);
        JsonArray values(*this);
        std::forward<F>(write)(values);
        _json += ']';
        _first = first;
    }

    std::string _json = "{";
    bool _first = true;
    EnumStringFormat _key_format;
};

class JsonArray {
public:
    JsonArray(const JsonArray&) = delete;

    JsonArray& operator=(const JsonArray&) = delete;

    void string(std::string_view value);

    void boolean(bool value);

    template <JsonInteger T>
    void integer(T value) {
        _owner.value_prefix();
        _owner.append_integer(value);
    }

    template <typename F>
        requires std::invocable<F, JsonObject&>
    void object(F&& write) {
        _owner.value_prefix();
        _owner.write_object(std::forward<F>(write));
    }

    template <typename F>
        requires std::invocable<F, JsonArray&>
    void array(F&& write) {
        _owner.value_prefix();
        _owner.write_array(std::forward<F>(write));
    }

private:
    friend class JsonObject;
    explicit JsonArray(JsonObject& owner);

    JsonObject& _owner;
};

} // namespace rhadar
