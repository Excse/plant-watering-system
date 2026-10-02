idf_component_register(
    SRCS "main.cpp" "mqtt_identity.cpp" "moisture_sensor.cpp"
    INCLUDE_DIRS "."
    PRIV_REQUIRES driver esp_adc esp_wifi esp_event esp_netif nvs_flash mqtt
    EMBED_TXTFILES "discovery.json" "moisture_discovery.json")

# Integrate the rhadar submodule through its native CMake target.
set(RHADAR_BUILD_TESTING OFF)
set(RHADAR_INSTALL OFF)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../lib/rhadar"
    "${CMAKE_CURRENT_BINARY_DIR}/rhadar")
target_link_libraries(${COMPONENT_LIB} PRIVATE rhadar::rhadar)
