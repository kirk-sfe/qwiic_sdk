/**
 * Copyright (c) 2020 SparkFun Electronics
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"


#include "sparkfun/bme280.h"
/* 
  Hack of a Arduino Qwiic library - BME280
*/

#define UART_ID uart0
#define BAUD_RATE 115200
#define UART_TX_PIN 0 
#define UART_RX_PIN 1

BME280 mySensor;

int main() {

	const uint LED_PIN = 25;
    gpio_init(LED_PIN);
    gpio_dir(LED_PIN, GPIO_OUT);
	gpio_put(LED_PIN, 1);

    // Set up our UART with the required speed.
    uart_init(UART_ID, BAUD_RATE);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
	//setup_default_uart();

	printf("Starting BME280 Test\n\n");

    // Init the system 

    if(!mySensor.begin()){
    	printf("Error starting the sensor/i2c detection of sensor. Aboarting\n");
  		return 0;
    }

    while (1) {
        gpio_put(LED_PIN, 1);
        sleep_ms(250);

        printf("LOOP \n");
        
        printf("Humidity: %f\n", mySensor.readFloatHumidity());

  		printf(" Pressure: %f\n", mySensor.readFloatPressure());

  		printf(" Alt: %f\n", mySensor.readFloatAltitudeFeet());

  		printf(" Temp: %f\n", mySensor.readTempF());

    	gpio_put(LED_PIN, 0);
        sleep_ms(2000);

    }

    return 0;
}
