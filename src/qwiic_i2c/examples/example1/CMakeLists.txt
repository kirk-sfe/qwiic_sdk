add_executable(bme280_example1
    bme280_example1.cpp
)

# Pull in our (to be renamed) simple get you started dependencies
target_link_libraries(bme280_example1 sparkfun_bme280 pico_stdlib hardware_i2c)

# create map/bin/hex file etc.
pico_add_extra_outputs(bme280_example1)
