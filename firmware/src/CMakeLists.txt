idf_component_register(
    SRCS "main.cpp" "mqtt_identity.cpp" "moisture_sensor.cpp"
    INCLUDE_DIRS "."
    PRIV_REQUIRES driver esp_adc esp_wifi esp_event esp_netif nvs_flash mqtt
    EMBED_TXTFILES "discovery.json")
