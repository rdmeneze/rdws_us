# Services CMakeLists.txt
# This file manages all services in the project

# Add all service subdirectories
add_subdirectory(example_service)

# Create services output directory
set(SERVICES_OUTPUT_DIR ${CMAKE_BINARY_DIR}/services)
file(MAKE_DIRECTORY ${SERVICES_OUTPUT_DIR})

# Custom target to copy all service binaries to a common location
add_custom_target(copy_services_binaries ALL
    COMMENT "Copying service binaries to services directory"
)

# Copy example_service binary to services directory as example_service
add_custom_command(TARGET copy_services_binaries POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:example_service>
        ${SERVICES_OUTPUT_DIR}/example_service
    COMMENT "Copying example_service to services directory as example_service"
)

# Set dependency to ensure example_service is built before copying
add_dependencies(copy_services_binaries example_service)