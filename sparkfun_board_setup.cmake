
# Add the sparkfun defined boards to the build system.
#
# This file is running at the root of the sparkfun sdk and mimics what happens during
# setup of the pico-sdk

# add our board definition directory to the pico sdk board define search pico_find_in_paths
list(APPEND PICO_BOARD_HEADER_DIRS ${CMAKE_CURRENT_LIST_DIR}/boards/include/boards)

# add location for board specific cmake files - if they exist
list(APPEND PICO_BOARD_CMAKE_DIRS ${CMAKE_CURRENT_LIST_DIR}/boards)

# Add the location of the board definition header files to the build system search path
list(APPEND PICO_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/boards/include) # so boards/foo.h can be explicitly included