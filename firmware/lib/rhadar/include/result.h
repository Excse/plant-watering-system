#pragma once

#include <expected>
#include <string>

#include "utils/enum.h"

namespace rhadar {

#define VALIDATION_ERROR_CODES(X, TargetEnum)                                   \
    X(TargetEnum, MissingUniqueId,             "missing_unique_id")             \
    X(TargetEnum, MissingStateTopic,           "missing_state_topic")           \
    X(TargetEnum, NegativeValue,               "negative_value")                \
    X(TargetEnum, IncompatibleOptions,         "incompatible_options")          \
    X(TargetEnum, IncompatibleLastReset,       "incompatible_last_reset")       \
    X(TargetEnum, IncompatibleStateClassUnit,  "incompatible_state_class_unit") \
    X(TargetEnum, InvalidEntityCategory,       "invalid_entity_category")       \
    X(TargetEnum, MissingConnectionType,       "missing_connection_type")       \
    X(TargetEnum, MissingConnectionIdentifier, "missing_connection_identifier") \
    X(TargetEnum, MissingDeviceIdentity,       "missing_device_identity")       \
    X(TargetEnum, EmptyDeviceIdentifier,       "empty_device_identifier")       \
    X(TargetEnum, MissingOriginName,           "missing_origin_name")           \
    X(TargetEnum, InvalidDiscoveryPrefix,      "invalid_discovery_prefix")      \
    X(TargetEnum, InvalidNodeId,               "invalid_node_id")               \
    X(TargetEnum, InvalidObjectId,             "invalid_object_id")             \
    X(TargetEnum, InvalidQos,                  "invalid_qos")                   \
    X(TargetEnum, InvalidAvailabilityTopic,    "invalid_availability_topic")    \
    X(TargetEnum, MissingComponents,           "missing_components")            \
    X(TargetEnum, InvalidComponentId,          "invalid_component_id")          \
    X(TargetEnum, DuplicateComponentId,        "duplicate_component_id")
DEFINE_ENUM(ValidationErrorCode, VALIDATION_ERROR_CODES)

struct ValidationError {
    ValidationErrorCode code;
    std::string field;
    std::string message;
};

template <typename T>
using Result = std::expected<T, ValidationError>;

} // namespace rhadar
