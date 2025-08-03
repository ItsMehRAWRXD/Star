cmake_minimum_required(VERSION 3.15)
project(Rawr VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# Platform-specific settings
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    find_package(Threads REQUIRED)
endif()

# Find required packages
find_package(Threads REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Add subdirectories
add_subdirectory(core)
add_subdirectory(plugins)
add_subdirectory(examples)
add_subdirectory(tests)

# Main executable
add_executable(rawr
    main.cpp
    core/framework/unified_framework.cpp
)

# Link libraries
target_link_libraries(rawr
    core_framework
    time_manipulator
    calc_replacer
    malware_hunter
    predator_armor
    malware_pacman
    power_manager
    multi_persona
    math_gatekeeper
    bouncer_mode
    pixel_evasion
    bitcoin_miner
    offline_ai
    multiverse_system
    Threads::Threads
)

# Set output directory
set_target_properties(rawr PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Install rules
install(TARGETS rawr
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Install configuration files
install(DIRECTORY configs/
    DESTINATION etc/rawr
    FILES_MATCHING PATTERN "*.yaml" PATTERN "*.json" PATTERN "*.conf"
)

# Install documentation
install(DIRECTORY documentation/
    DESTINATION share/rawr/doc
    FILES_MATCHING PATTERN "*.md" PATTERN "*.txt"
)

# Install scripts
install(DIRECTORY scripts/
    DESTINATION share/rawr/scripts
    FILES_MATCHING PATTERN "*.sh" PATTERN "*.py" PATTERN "*.bat"
)

# Enable testing
enable_testing()

# Add tests
add_test(NAME CoreFrameworkTest COMMAND rawr --test-core)
add_test(NAME PluginTest COMMAND rawr --test-plugins)
add_test(NAME IntegrationTest COMMAND rawr --test-integration)

# Set test properties
set_tests_properties(CoreFrameworkTest PROPERTIES
    TIMEOUT 30
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set_tests_properties(PluginTest PROPERTIES
    TIMEOUT 60
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set_tests_properties(IntegrationTest PROPERTIES
    TIMEOUT 120
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/RawrConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# CPack configuration
set(CPACK_PACKAGE_NAME "Rawr")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Advanced Cybersecurity Learning Platform")
set(CPACK_PACKAGE_VENDOR "Rawr Security Team")
set(CPACK_PACKAGE_CONTACT "rawr@security.com")

if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_NSIS_DISPLAY_NAME "Rawr Cybersecurity Platform")
    set(CPACK_NSIS_PACKAGE_NAME "Rawr")
elseif(UNIX)
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Rawr Security Team")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libstdc++6")
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
endif()

include(CPack)

# Print configuration summary
message(STATUS "Rawr Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Build Directory: ${CMAKE_BINARY_DIR}")
message(STATUS "  Source Directory: ${CMAKE_SOURCE_DIR}")

# Custom targets
add_custom_target(docs
    COMMAND ${CMAKE_COMMAND} -E echo "Generating documentation..."
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/docs
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating documentation"
)

add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/build
    COMMENT "Cleaning all build artifacts"
)

# Development helpers
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_custom_target(format
        COMMAND find ${CMAKE_SOURCE_DIR} -name "*.cpp" -o -name "*.h" | xargs clang-format -i
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Formatting source code"
    )
    
    add_custom_target(lint
        COMMAND cppcheck --enable=all --std=c++17 ${CMAKE_SOURCE_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running static analysis"
    )
endif()