/*
 * table.c
 *
 *  Created on: Mar 27, 2020
 *      Author: sagar
 */

#include <statemachine_table.h>
#include <stdio.h>

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

static bool timer_enabled = false;

extern bool timeout;

// start table state machine
int Tabledriven_StateMachine_init()
{
	count = 0;
	state_arr[0].func_p(&state_arr[0]);  //event one
	return 0;
}

extern int state_1(const struct state *s_p)
{
	b=read_full_xyz();//reading the accmeter values
	Log_string("STATE_READ_XYZ_STATE\r\n", TABLEDRIVEN_STATEMACHINE_INIT, LOG_TEST);

	if(b==1) //checking if read_full_xyz has read values
	{
		LED_flash(GREEN,1);
		state_arr[1].func_p(&state_arr[1]); //Enter second event for displaying these values
	}

	else
	{
		Log_string("STATE_SENSOR_DISCONNECT_STATE\r\n", TABLEDRIVEN_STATEMACHINE_INIT, LOG_TEST);
		LED_on(RED);
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
	PRINTF("The number of times it has entered this state machine is %d\r\n\n",count+1);

	//checking systick
	if(!timer_enabled){
		SysTick_enable();
		timer_enabled = true;
	}
	while(!timeout)
	{

	int op=Slider_scan();//scanning the slider value
	Delay(50000);
	if(op>RIGHT_LOWER && op<RIGHT_HIGHER) //if it is right slider touch it will end program!
	{
		count=50;//just for reference 50 is used

	}
	else if(op>LEFT_LOWER && op<LEFT_HIGHER ) //if left slider it will go into next SM
	{
	   count=100;//just for reference 100 is used //100=left and 50=right
	}

	}

	if(timeout)//making IRQ wait to stop
	{
		timeout = false;
	}

	if(count<4) //if number of time it entered is less than 6 times it should read xyz again
	{
		count=count+1;
		state_arr[0].func_p(&state_arr[0]);//repeat the state machine if it entered less than 5 times
	}

	else if(count==4) //if there is timeout 6
	{
#ifdef TESTING_MODE
		UCUNIT_TestcaseBegin("Table-driven I2C State Machine Timeout 6 Test\r\n");
		UCUNIT_CheckIsEqual(4, count);
		UCUNIT_TestcaseEnd();
#endif
		Log_string("Timeout six times. Go to other state machine\r\n", TABLEDRIVEN_STATEMACHINE_INIT, LOG_STATUS);
		end=2;
	}

	else if(count==100)
	{
		Log_string("Left transition. Go to other state machine\r\n", TABLEDRIVEN_STATEMACHINE_INIT, LOG_STATUS);
		end=2;//for the left slider to go to SPi state machine
	}

	else if(count==50)
	{
		Log_string("Right transition. End program\r\n", TABLEDRIVEN_STATEMACHINE_INIT, LOG_STATUS);
		state_arr[3].func_p(&state_arr[3]);//go to last state to end the state machine
	}
	return 0;
}

extern int state_4(const struct state *s_p)//last even to end the state machine
{
	LED_on(RED);
	end=1;
    return 0;
}
