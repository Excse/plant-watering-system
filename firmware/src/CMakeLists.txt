idf_component_register(
    SRCS "main.c" "mqtt_identity.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES driver esp_wifi esp_event esp_netif nvs_flash mqtt
    EMBED_TXTFILES "discovery.json")
