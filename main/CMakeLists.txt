idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES freertos driver esp_wifi nvs_flash esp_netif)