/*
 * table.c
 *
 *  Created on: Mar 27, 2020
 *      Author: sagar
 */

#include"table.h"
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"

#include <math.h>

#include "slider.h"
#include "led_control.h"
#include <stdbool.h>
#include "global_defines.h"
#include"i2c_interrupt.h"


int b;
volatile int count;
extern int end;

int Tabledriven_StateMachine_init()
{

state_arr[0].func_p(&state_arr[0]);  //event one
return 0;
}

extern int state_1(const struct state *s_p)
{
	b=read_full_xyz();
//	Control_RGB_LEDs(0, 1 , 0);

	if(b==1) //checking if read_full_xyz has read values
	{
//		LED_on(GREEN);
//	    Delay(100);
//	    LED_off(GREEN);
		LED_flash(GREEN,1);
		state_arr[1].func_p(&state_arr[1]); //Enter second event for displaying these values

	}

	else
	{
		printf("Sensor disconnect and END\r\n");
		LED_on(RED);
	//	Control_RGB_LEDs(1, 0 , 0);
		state_arr[3].func_p(&state_arr[3]);//go to last state to end the state machine
	}
    return 1;



}

extern int state_2(const struct state *s_p)
{
	dispaly_values();//printing out the xyz values
	state_arr[2].func_p(&state_arr[2]);//On Complete of display, the state will change to Wait/Poll Slider
	return 0;
}

extern int state_3(const struct state *s_p)
{
printf("The number of times it has entered the state machine %d\r\n",count);


//wait for 3 second and later check for slider status
		Delay(900);

		int op=Slider_scan();
		printf("%d\n",op);
			if(op>690 && op<2700) //if it is right slider touch it will end program!
			{

				printf("END");
				state_arr[3].func_p(&state_arr[3]);//go to last state to end the state machine
			}

			else if(count<6) //if number of time it entered is less than 6 times it should read xyz again
		{
				count=count+1;
				state_arr[0].func_p(&state_arr[0]);
		}

			else if(count==6) //or left slider
			{
				printf("Enter previous state machine\r\n");
				end=2;
			}


	return 0;
}

extern int state_4(const struct state *s_p)
{
	LED_on(RED);

	end=1;
    return 0;
}
