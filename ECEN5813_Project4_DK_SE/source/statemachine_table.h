/*
 * table.h
 *
 *  Created on: Mar 27, 2020
 *      Author: sagar
 */

#ifndef TABLE_H_
#define TABLE_H_



#endif /* TABLE_H_ */


#include <stdio.h>




// enum for state state machine states
typedef enum{
	STATE_READ_XYZ,
	STATE_TABLE_DISPLAY,

} SensorStateTable_t;





struct state;

/* State function prototype */
typedef int (state_func)(const struct state *);

/* Structure containing state function and data */
struct state
{
    int         state;
    int         data;
    state_func  *func_p;
};


/* State function declarations */
extern int state_1(const struct state *);
extern int state_2(const struct state *);
extern int state_3(const struct state *);
extern int state_4(const struct state *);

/* State table */
static const struct state state_arr[] =
{
    { 1, 0, state_1 },
    { 2, 0, state_2 },
    { 3, 0, state_3 },
    { 4, 0, state_4 },
    { 0, 0, NULL }
};

int Tabledriven_StateMachine_init(void);

