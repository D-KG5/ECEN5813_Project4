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
#include "logger.h"
#include <stdbool.h>
#include "timer.h"
int b;
volatile int count;
extern int end;

//static bool timer_enabled = false;


int Tabledriven_StateMachine_init()
{

state_arr[0].func_p(&state_arr[0]);  //event one
return 0;
}

extern int state_1(const struct state *s_p)
{
	b=read_full_xyz();
	Log_string("STATE_READ_XYZ_STATE\r\n", TABLEDRIVEN_STATEMACHINE_INIT, LOG_TEST);
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
		Log_string("STATE_SENSOR_DISCONNECT_STATE\r\n", TABLEDRIVEN_STATEMACHINE_INIT, LOG_TEST);
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
PRINTF("The number of times it has entered this state machine is %d\r\n",count);


//wait for 3 second and later check for slider status
		Delay(3000);
		//checking systick
//		if(!timer_enabled){
//			SysTick_enable();
//			timer_enabled = true;
//		}

		int op=Slider_scan();
	//	printf("%d\n",op);
			if(op>690 && op<2700) //if it is right slider touch it will end program!
			{

				Log_string("Right transition. End program\r\n", TABLEDRIVEN_STATEMACHINE_INIT, LOG_STATUS);
				state_arr[3].func_p(&state_arr[3]);//go to last state to end the state machine
			}

			else if(count<6) //if number of time it entered is less than 6 times it should read xyz again
		{
				count=count+1;
				state_arr[0].func_p(&state_arr[0]);
		}

			else if(count==6 || (op>35 && op<680)) //if there is timeout 6 or left slider it will go into next SM
			{
#ifdef TESTING_MODE
				UCUNIT_TestcaseBegin("Table-driven I2C State Machine Timeout 6 Test\r\n");
				UCUNIT_CheckIsEqual(6, count);
				UCUNIT_TestcaseEnd();
#endif
               if(count==6)
               {
				Log_string("Timeout six times. Go to other state machine\r\n", TABLEDRIVEN_STATEMACHINE_INIT, LOG_STATUS);
               }
               else
               {
            	   Log_string("Left transition. Go to other state machine\r\n", TABLEDRIVEN_STATEMACHINE_INIT, LOG_STATUS);
               }
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
