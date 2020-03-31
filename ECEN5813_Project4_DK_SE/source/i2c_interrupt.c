/*
 * i2c_interrupt.c
 *
 *  Created on: Mar 29, 2020
 *      Author: sagar
 */
#include <stdio.h>
#include <stdint.h>
#include "slider.h"
#include "i2c_poll.h"
#include <stdbool.h>
#include "global_defines.h"
#include"i2c_interrupt.h"

int a[6];
uint8_t data[6];

int16_t temp[3];
int interrupt_trasnmit;
int16_t acc_X, acc_Y, acc_Z;

void enbaleinterrupt(void)
{
	__disable_irq();
	//set interrupt
    I2C0->C1 |= (I2C_C1_IICIE_MASK);
	//enable i2c and set to master mode
    NVIC->ICPR[0] |= 1 << ((I2C0_IRQn)%32);
    NVIC->ISER[0] |= 1 << ((I2C0_IRQn)%32);

    __enable_irq();
}

void i2c_Transmit(void)
{
	I2C0->C1 |= I2C_C1_MST_MASK;
	I2C0->D = ((SLAVE_ADDRESS << 1) | READ);


}

int read_full_xyz()
{
	i2c_starti();
	i2c_readsetupi( SLAVE_ADDRESS, REG_XHI,0);

	return 1;

}

void i2c_starti()
{
	I2C_TRAN;							/*set to transmit mode */
	I2C_M_START;					/*send start	*/
}

void i2c_readsetupi(uint8_t dev, uint8_t address,uint8_t isLastRead)
{
	//uint8_t data;
	I2C0->D = dev;			  /*send dev address	*/
//	while(interrupt_clear==0);							/*wait for completion */

	I2C0->D =  address;		/*send read address	*/
	I2C_WAIT							/*wait for completion */

	I2C_M_RSTART;				   /*repeated start */
	I2C0->D = (dev|0x1);	 /*send dev address (read)	*/
	I2C_WAIT							 /*wait for completion */

	I2C_REC;						   /*set to receive mode */
	if(isLastRead)	{
			NACK;								/*set NACK after read	*/
		} else	{
			ACK;								/*ACK after read	*/
		}

	enbaleinterrupt();

//	data = I2C0->D;



}

void I2C0_IRQHandler(void)
{
	I2C0->S  |= I2C_S_IICIF_MASK;// Clear interrupt flag

	static int i=0;

	interrupt_trasnmit=0;

	//lock_detect = 0;

	if(i==4)	{
		NACK;								/*set NACK after read	*/
	} else	{
		ACK;								/*ACK after read	*/
	}

	         			/*dummy read	*/
								/*wait for completion */

	if(i==5)	{
		I2C_M_STOP;					/*send stop	*/
	}

	data[i] = I2C0->D;				/*read data	*/
     a[i]=I2C0->D;

	if(i==5)
	{
		interrupt_trasnmit=1;
		i=-1;

		I2C0->C1 &=~ (I2C_C1_IICIE_MASK);
		//diable nvic

	}
i++;


}

void dispaly_values(void)
{
//	printf("register_value %d\r\n",a[0]);

	for ( int k=0; k<3; k++ ) {
		temp[k] = (int16_t) ((a[2*k]<<8) | a[2*k+1]);
	}

	// Align for 14 bits
	acc_X = temp[0]/4;
	acc_Y = temp[1]/4;
	acc_Z = temp[2]/4;


	printf("X: %d Y: %d Z: %d\r\n",acc_X,acc_Y,acc_Z);
	printf("Average %d\r\n",((acc_X+acc_Y+acc_Z)/3) );
	//	//https://code4coding.com/c-programfind-smallest-of-three-numbers-using-function/

	    if(acc_X<acc_Y){//compare num1 and num2
	            if(acc_X<acc_Z){//compare num1 and num3
	         printf("Low number is: %d\r\n",acc_X);
	         if(acc_Y>acc_Z)
			 {
	        	 printf("High number is: %d\r\n",acc_Y);
			 }
	         else
	         {
	        	 printf("High number is: %d\r\n",acc_Z);
	         }
	            }
	            else{
	                printf("Low number is: %d\r\n",acc_Z);
	   	         if(acc_Y>acc_X)
	   			 {
	   	        	 printf("High number is: %d\r\n",acc_Y);
	   			 }
	   	         else
	   	         {
	   	        	 printf("High number is: %d\r\n",acc_X);
	   	         }

	            }
	    }
	    else{
	        if(acc_Y<acc_Z){//compare num2 and num1
	            printf("Low number is: %d\r\n",acc_Y);

	   	         if(acc_Z>acc_X)
	   			 {
	   	        	 printf("High number is: %d\r\n",acc_Z);
	   			 }
	   	         else
	   	         {
	   	        	 printf("High number is: %d\r\n",acc_X);
	   	         }
	        }
	        else{
	            printf("Low number is: %d\r\n",acc_Z);


	   	         if(acc_Y>acc_X)
	   			 {
	   	        	 printf("High number is: %d\r\n",acc_Y);
	   			 }
	   	         else
	   	         {
	   	        	 printf("High number is: %d\r\n",acc_X);
	   	         }


	        }
	}

}

