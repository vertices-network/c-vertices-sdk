# ## Certs
# * Embed the server root certificate into the final binary
# * (If this was a component, we would set COMPONENT_EMBED_TXTFILES here.)

idf_component_register(SRCS esp32_example.c http_esp.c
        INCLUDE_DIRS "."
        EMBED_TXTFILES algoexplorer_root_cert.pem
        REQUIRES)

# Add account keys from binary files in the Firmware image
# ⚠️ This is not a secure way to handle keys!
target_add_binary_data(${COMPONENT_LIB} "private_key.bin" "BINARY"
        RENAME_TO private_key_bin)
target_add_binary_data(${COMPONENT_LIB} "public_b32.txt" "BINARY" RENAME_TO account_address)

# force_linker_inclusion function is added as an undefined symbol because otherwise the
# linker will ignore the http functions in `http_esp.c` as symbols are already resolved by weak functions
# from the Vertices SDK
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u force_linker_inclusion")