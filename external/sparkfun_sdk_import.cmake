# This is a copy of <SPARKFUN_SDK_PATH>/external/pico_sdk_import.cmake

# This can be dropped into an external project to help locate this SDK
# It should be include()ed prior to project()


if (DEFINED ENV{SPARKFUN_SDK_PATH} AND (NOT SPARKFUN_SDK_PATH))
    set(SPARKFUN_SDK_PATH $ENV{SPARKFUN_SDK_PATH})
    message("Using SPARKFUN_SDK_PATH from environment ('${SPARKFUN_SDK_PATH}')")
endif ()

set(SPARKFUN_SDK_PATH "${SPARKFUN_SDK_PATH}" CACHE PATH "Path to the SPARKFUN SDK")

get_filename_component(SPARKFUN_SDK_PATH "${SPARKFUN_SDK_PATH}" REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")
if (NOT EXISTS ${SPARKFUN_SDK_PATH})
    message(FATAL_ERROR "Directory '${SPARKFUN_SDK_PATH}' not found")
endif ()

set(SPARKFUN_SDK_INIT_BOARD_FILE ${SPARKFUN_SDK_PATH}/sparkfun_board_setup.cmake)
if (NOT EXISTS ${SPARKFUN_SDK_INIT_BOARD_FILE})
    message(FATAL_ERROR "Directory '${SPARKFUN_SDK_PATH}' does not appear to contain the SPARKFUN SDK")
endif ()

set(SPARKFUN_SDK_PATH ${SPARKFUN_SDK_PATH} CACHE PATH "Path to the Sparkfun SDK" FORCE)

# Run board setup
include(${SPARKFUN_SDK_INIT_BOARD_FILE})

# Add the SDK sub directory tree to the build system
add_subdirectory(${SPARKFUN_SDK_PATH} sparkfun_sdk)

