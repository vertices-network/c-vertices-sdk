# ## Certs
# * Embed the server root certificate into the final binary
# * (If this was a component, we would set COMPONENT_EMBED_TXTFILES here.)

idf_component_register(SRCS "esp32_example.c"
        INCLUDE_DIRS "."
        EMBED_TXTFILES algoexplorer_root_cert.pem
        REQUIRES)

# Add account keys from binary file in the Firmware image
# ⚠️ This is not a secure way to handle keys!
target_add_binary_data(${COMPONENT_LIB} "private_key.bin" "BINARY"
        RENAME_TO private_key_bin)