# ## Certs
# * Embed the server root certificate into the final binary
# * (If this was a component, we would set COMPONENT_EMBED_TXTFILES here.)
# ## Requires
# This is not the `main` component but part of EXTRA_COMPONENT_DIRS
# so we need to add required other components (directory names)

idf_component_register(SRCS "esp_http_client_example.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES howsmyssl_com_root_cert.pem
                    postman_root_cert.pem
                    REQUIRES)

# idf_component_register(SRCS "esp_http_client_example.c"
# INCLUDE_DIRS "."
# EMBED_TXTFILES howsmyssl_com_root_cert.pem
# postman_root_cert.pem
# REQUIRES nvs_flash protocol_examples_common esp-tls esp_http_client vertices)