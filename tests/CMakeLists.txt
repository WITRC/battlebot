cmake_minimum_required(VERSION 3.13)

project(motor_tests C)

# This is for native compilation, not cross-compile for Pico
set(CMAKE_C_STANDARD 11)

# Define UNIT_TESTING to enable conditional compilation
add_definitions(-DUNIT_TESTING)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hardware
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/config
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
    ${CMAKE_CURRENT_SOURCE_DIR}/support
    ${CMAKE_CURRENT_SOURCE_DIR}/CMock/vendor/unity/src
)

# Unity test framework source
set(UNITY_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/CMock/vendor/unity/src/unity.c
)

# Mock hardware implementation
set(MOCK_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/support/mock_pico_hardware.c
)

# Source files under test
set(SOURCE_UNDER_TEST
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hardware/motor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hardware/motor_controller.c
)

# Test files
set(TEST_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/all_tests.c
)

# Create test executable
add_executable(motor_tests
    ${UNITY_SRC}
    ${MOCK_SRC}
    ${SOURCE_UNDER_TEST}
    ${TEST_SRC}
)

# Link math library (for abs() function)
target_link_libraries(motor_tests m)
