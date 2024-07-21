# Define the custom target to generate protocol files
add_custom_target(generate-connector-protocol ALL
    COMMAND ${CMAKE_COMMAND} -E env make generate-protocol
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/python
    COMMENT "Generating protocol files"
)

# Ensure the protocol files are generated before compiling sources
add_dependencies(rofi_nimble generate-connector-protocol)