# sparkfun_sdk experiment for the rp2040

This is a prototype/experiment on how to port and integrate SparkFun qwiic sensor drivers into the RP2040/Pico build environment. 

This is just an experiment. While the basic operational will use similar for the final library, the underlying code port will be different – with an effort to use the same code on both Arduino and Pico environments.

Note: This has only been tested using the command line interface.
## Contents

* [SRP2040 General Operation](#rp2040-general-operation)
* [General Notes on the SDK](#general-notes-on-the-sdk)
* [Supported Drivers/Boards](#ported-drivers)

## RP2040 General Operation
### Firmware Flashing
The rp2040 uses a UF2 based bootloader, which implements a USB mass storage devices (behaves like a usb thumb drive) to flash the system. When in this mode, the desired firmware is flashed onto the target device by dragging the firmware .uf2 file to the target device. Once the firmware is copied to the device, the system boots using the new firmware.

1) To enter UF2 flash mode (have the board appear as a USB flash drive), the method available to all boards is the following:
  * Power down the board (normally unplug the USB cable from the board)
  * Power up the board (i.e. plug in the usb cable) while holding the boot button

2)	Entering UF2 on SparkFun boards (MicroMod and Pro Micro currently)
  * While the board is powered, press and release the boot and reset buttons simultaneously.

    This provides SparkFun boards an advantage over the Pico board – no unplug-and-plug cycle to enter UF2 bootloader mode.

### Text Output via STDIO and USB
By default, the pico SDK uses a UART to output text generated using the stdio (printf()) within the SDK. From the SDKs standpoint, this is more efficient and keeps the USB device free for other functionality.

However, from a introductory rp2040 developer standpoint, establishing an additional UART connection between the board and development system, or using a single wire debug (SWD) interface, is an additional burden. 

In the pico SDK, the stdio output can be redirected to the USB connection (usb-to-serial) by doing the following:

In the firmware source code, setup the stdio subsystem using the ```C stdio_init_all();``` function call. This function is used to setup stdio for any output device (UART, USB or both).

Hello World Example:
```c
#include <stdio.h>
#include "pico/stdlib.h"
	

int main() {
    stdio_init_all();
    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
    return 0;
}
```
The device to output to is set in the build system via the projects CMakeLists.txt file. The functions ```pico_enable_stdio_usb()``` and ```pico_enable_stdio_uart()``` are used to control the output. Calling parameters to these functions are:
  
  pico_enable_sudio_<deivce>(<project name> <enable-disable>)
	Where:
  	project_name – the name used for this project – the first parameter to add_executable()

	  enable-disable – pass in 1 to enable that subsystem, 0 to disable it.

So for the above hello world example, to output to USB, the CMakeLists.txt file is:
```
add_executable(hello_usb
	        hello_usb.c
	        )
	
# Pull in our pico_stdlib which aggregates commonly used features
target_link_libraries(hello_usb pico_stdlib)	

# enable usb output, disable uart output
pico_enable_stdio_usb(hello_usb 1)
pico_enable_stdio_uart(hello_usb 0)
	
# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(hello_usb)
```

The advantage of controlling output via the build system is that output changes can be made without editing source code.

## General Notes on the SDK
All the device libraries are ported from the existing Arduino libraries (the RFID library was a mostly a rewrite though...). The code was hacked to make it work for the most part. Arduino specifics were removed and replaced with pico/c++ as needed.

The i2c communication was removed from all libraries and encapsulated in a common object. 

In general the goal was to make the system work and determine differences between the Arduino and Pico frameworks. This information shall be used when doing the final port/development for the Pico ecosystem. 

### Status
* Developed and everything tested on a mac
* Tested on Windows - just built the bme280 example to verify
* All libraries are tested for build and general operation - actual validation has not occured
* the MicroOLED port is in progress and doesn't work.

## Use of the SDK
* Setup the pico build environment as outlined in the Pico SDK documentation.
* Clone this repository into the same directory that the pico SDK was installed. 
* Define the environment variable SPARKFUN_SDK_PATH to the sparkfun-sdk path. NOTE: Use absolute paths
Example:
```sh
  export SPARKFUN_SDK_PATH=/Users/sparky/Development/pico/sparkfun-sdk
  ```
  
That's all the setup needed. 

### Specifying the Target Board
The target board is set by defining the PICO_BOARD variable in CMake. To do this at the command line, use the following pattern:
```sh
cmake -D PICO_BOARD=<name>
```

The names for the SparkFun RP2040 boards are:
* sparkfun_promicro
* sparkfun_micromod
* sparkfun_thingplus

So to build a binary for the RP2040 MicroMod, use the following cmake command:
```sh
cmake -D PICO_BOARD=sparkfun_micromod 
```

## SDK Structure
The general SDK structure mimics that of the pico-sdk. The contents have the following general structure:

### boards/…
Includes header files that defines the details of a board. 

### external/.
Hold the setup file projects copy/use to use the SparkFun SDK

### src/.
Contains individual “interface” libraries the SDK provides. Think of an interface library as a Arduino library – it contains the implementation of a specific set of functionality, like a qwiic device driver.

The src/LIBRARY_NAME/ folder contains the implementation of the library. 

#### src/LIBRARY_NAME/include
This includes  the public header file for the library. Note this has a structure of ```include/sparkfun```, so that the SDK headerfiles are all contained in the “sparkfun” folder.  Example:
```c
#include “sparkfun/qwiic_bme280.h”
```
#### src/LIBRARY_NAME/examples
This folder contains the examples for the specific library.

To use these examples, they should be copied to a location outside of this SDK folder.

### Example
To build the BME280 example, perform the following steps:
* Create a test directory
* Copy the files from the example1 directory in the SDK to this folder
    ```sh cp $SPARKFUN_SDK_PATH/src/qwiic_bme280/examples/example1/* . ```
* Copy the SDK config files into this directory for both the pico and sparkfun sdks
```sh
         cp $SPARKFUN_SDK_PATH/external/sparkfun_sdk_import.cmake .
         cp $PICO_SDK_PATH/external pico_sdk_import.cmake .
```
* create a build directory and move into it
```sh
mkdir build
cd build
```
* Run cmake, specifying the target board
```sh
cmake -D PICO_BOARD=sparkfun_micromod ..
```
On windows the option ```-G "NMake Makefiles``` should be included

* Make the binary
```sh
make j4
```
Or on Windows
```sh
nmake
```
* Upload the new binary to the target board (uf2 file)

## Ported Drivers
The following drivers/boards are supported by this prototype
* Qwiic AK975X
* BME280
* CCS811
* Qwiic Joystick
* Qwiic Keypad
* Qwiic Relay
* Qwiic RFID
* Qwiic Twist
* Qwiic Micro OLED
* Qwiic VCNL4040
  
