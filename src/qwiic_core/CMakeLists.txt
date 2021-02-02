
# is this interface library defined yet?

if (NOT TARGET sparkfun_qwiic_core)
    add_library(sparkfun_qwiic_core INTERFACE)

    # library source files
    target_sources(sparkfun_qwiic_core INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/qwiic_i2c.cpp
    )

    # Where is this include files for this library. Note, this include directory uses
    # the directory structure in the include directory to set a "sparkfun/<library>"
    # pattern for the sparkfun sdk.
    target_include_directories(sparkfun_qwiic_core INTERFACE ${CMAKE_CURRENT_LIST_DIR})

    # libraries this library depends on. 
    target_link_libraries(sparkfun_qwiic_core INTERFACE
    	hardware_i2c
        pico_stdlib
    )

endif()
