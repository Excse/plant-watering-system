#pragma once

#include <expected>
#include <string>

namespace rhadar {

enum class ValidationErrorCode {
    MISSING_UNIQUE_ID,
    MISSING_STATE_TOPIC,
    NEGATIVE_VALUE,
    INCOMPATIBLE_OPTIONS,
    INCOMPATIBLE_LAST_RESET,
    INCOMPATIBLE_STATE_CLASS_UNIT,
    INVALID_ENTITY_CATEGORY,
};

struct ValidationError {
    ValidationErrorCode code;
    std::string field;
    std::string message;
};

template <typename T>
using Result = std::expected<T, ValidationError>;

} // namespace rhadar
