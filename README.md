# ECEN5813_Project4
PES Project 4 for ECEN 5813. By Dhruva Koley and Sagar Eligar

# Contents
- ECEN5813_Project4_DK_SE
  - source
	- ECEN5813_Project4_DK_SE.c: main source file containing POST and state machine switching state machine
	- global_defines.h: header file that contains multiple flags to enable/disable logging, testing and macros
	- led_control.c: source file to initialize and control the LED
	- led_control.h: header file for led_control.c
	- logger.c: source file that contains logging utility functions
	- logger.h: header file for logger.c
	- i2c_interrupt.c: source file that contains i2c interrupt functions
	- i2c_interrupt.h: header file for i2c_interrupt.c
	- i2c_poll.c: source file that contains i2c poll functions
	- i2c_poll.h: header file for i2c_poll.c
	- sensor.c: source file that contains sensor (poll) functions
	- sensor.h: header file for sensor.c
	- slider.c: source file that contains slider functions
	- slider.h: header file for slider.c
	- spi.c: source file that contains spi (loopback poll) functions
	- spi.h: header file for spi.c
	- statemachine_spi.c: source file that contains state-driven SPI state machine functions
	- statemachine_spi.h: header file for statemachine_spi.c
	- statemachine_state.c: source file that contains state-driven I2C (poll) state machine functions
	- statemachine_state.h: header file for statemachine_state.c
	- statemachine_table.c: source file that contains table-driven I2C (interrupt) state machine functions
	- statemachine_table.h: header file for statemachine_table.c
	- timer.c: source file that contains SysTick timer functions
	- timer.h: header file for timer.c
- uCUnit
	- System.c: source file that containsKL25Z specific system changes to allow uCUnit to work
	- System.h: header file for System.h
	- uCUnit.h: header file containing unit test functions

# Notes
- The built-in accelerometer is used for this project.
- The SPI function is a simple loop-back.
- The slider touch state transitions are extremely glitchy. 
	- Right touch must be quick and forceful (leave the finger on it). 
	- Left touch must be extremely quick (quickly remove your finger).

# Environment
 - Windows 10
 - MCUXpresso v11.1.0
