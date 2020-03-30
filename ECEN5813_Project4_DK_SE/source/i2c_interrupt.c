/*
 * i2c_interrupt.c
 *
 *  Created on: Mar 29, 2020
 *      Author: sagar
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"


#include "slider.h"
#include "i2c_poll.h"
#include <stdbool.h>

#include "global_defines.h"


#include"i2c_interrupt.h"

int a[6];

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

void i2c_starti()
{
	I2C_TRAN;							/*set to transmit mode */
	I2C_M_START;					/*send start	*/
}

void i2c_readsetupi(uint8_t dev, uint8_t address,uint8_t isLastRead)
{
	uint8_t data;
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

	data = I2C0->D;



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

//		for ( int j=0; j<3; j++ ) {
//			temp[j] = (int16_t) ((data[2*j]<<8) | data[2*j+1]);
//		}
//
//		// Align for 14 bits
//		acc_X = temp[0]/4;
//		acc_Y = temp[1]/4;
//		acc_Z = temp[2]/4;
//
//		printf("X: %d Y: %d Z: %d\n ",acc_X,acc_Y,acc_Z);
		interrupt_trasnmit=1;
		i=-1;
//        printf("Average: %d",((acc_X+acc_Y+acc_Z)/3));

		I2C0->C1 &=~ (I2C_C1_IICIE_MASK);
		//diable nvic



	}











i++;

  //  printf("Hello\n");



}



void dispaly_values(void)
{
	printf("register_value %d\n",a[0]);
//	printf("register_value %d\n ",a[1]);
//	printf("register_value %d\n ",a[2]);
//	printf("register_value %d\n ",a[3]);
//	printf("register_value %d\n ",a[4]);
//	printf("register_value %d\n ",a[5]);

	for ( int k=0; k<3; k++ ) {
		temp[k] = (int16_t) ((a[2*k]<<8) | a[2*k+1]);
	}

	// Align for 14 bits
	acc_X = temp[0]/4;
	acc_Y = temp[1]/4;
	acc_Z = temp[2]/4;


	printf("X: %d Y: %d Z: %d\n ",acc_X,acc_Y,acc_Z);
	printf("Average %d\n",((acc_X+acc_Y+acc_Z)/3) );
	printf("Low values %d %d %d\n", a[1],a[3],a[5]);
	printf("High values %d %d %d\n", a[0],a[2],a[4]);

//	//https://code4coding.com/c-programfind-smallest-of-three-numbers-using-function/
//
//    if(acc_X<acc_Y){//compare num1 and num2
//            if(acc_X<acc_Z){//compare num1 and num3
//         printf("Smallest number is: %d\n",acc_X);
//            }
//            else{
//                printf("Smallest number is: %d\n",acc_Z);
//            }
//    }
//    else{
//        if(acc_Y<acc_Z){//compare num2 and num1
//            printf("Smallest number is: %d\n",acc_Y);
//        }
//        else{
//            printf("Smallest number is: %d\n",acc_Z);
//        }
//
//
//
//}


}

