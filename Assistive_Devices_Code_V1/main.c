#ifndef F_CPU
#define F_CPU 16000000ul
#endif

#include "h_bridge.h"
#include "servo.h"

#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>

//schakelaars
#define home_switch         PA0
#define top_switch          PA1
#define middel_switch       PA5

//bediningpaneel
#define home_knop           PA2
#define opbouw_knop         PA3
#define afbouw_knop         PA4
#define volgende_steiger    PA6
#define vorige_steiger      PA7


void init(void)
{
	init_h_bridge();
	init_servo();
}

int main(void)
{
	init();

	// output
	//bedingingpaneel
	PORTA &= (1<<home_knop);
	PORTA &= (1<<opbouw_knop);
	PORTA &= (1<<afbouw_knop);
	PORTA &= (1<<volgende_steiger);
	PORTA &= (1<<vorige_steiger);
	//schakelaars
	PORTA &= (1<<home_switch);
	PORTA &= (1<<top_switch);
	PORTA &= (1<<middel_switch);
	int status = 0;


	while (1)
	{
		switch(status)
		{
        case 0:
            if(!(PINA & (1<<home_switch)))
            {
                status = 2;
                break;
            }
            if (PINA &(1<<home_switch))
            {
                if(!(PINA & (1<<home_knop)))
                {
                    status = 1;
                    break;
                }
            }
            break;
        case 1:
            if(!(PINA & (1<<home_switch)))
            {
                status = 0;
                break;
            }
            h_bridge_set_percentage(-70);
            break;
        case 2:
            if(!(PINA & (1<<opbouw_knop)))
            {
                status = 3;
                break;
            }
            if(!(PINA & (1<<afbouw_knop)))
            {
                status = 4;
                break;
            }
            break;

            //Opbouwen
        case 3:
            servo1_set_percentage(50);
            servo2_set_percentage(50);
            status = 5;
            break;
        case 5:
            if (!(PINA & (1<<top_switch)))
            {
                h_bridge_set_percentage(0);
                status = 7;
                break;
            }
            h_bridge_set_percentage(70);
            break;
        case 7:
            if(!(PINA & (1<<volgende_steiger)))
            {
                status = 9;
            }
            break;
        case 9:
            if(!(PINA & (1<<middel_switch)))
               {
                   h_bridge_set_percentage(0);
                   status = 11;
                   break;
               }
               h_bridge_set_percentage(-70);
               break;
        case 11:
            servo1_set_percentage(-50);
            servo2_set_percentage(-50);
            status = 13;
            break;
        case 13:
            if (!(PINA & (1<<home_switch)))
            {
                h_bridge_set_percentage(0);
                status = 2;
                break;
            }
            h_bridge_set_percentage(-70);
            break;
        case 4:
            if (!(PINA &(1<<middel_switch)))
            {
                h_bridge_set_percentage(0);
                status = 6;
                break;
            }
            h_bridge_set_percentage(70);
            break;
        case 6:
            servo1_set_percentage(50);
            servo2_set_percentage(50);
            status = 8;
            break;
        case 8:
            if(!(PINA & (1<<top_switch)))
            {
                h_bridge_set_percentage(0);
                status = 10;
                break;
            }
            h_bridge_set_percentage(70);
            break;
        case 10:
            if(!(PINA &(1<<vorige_steiger)))
            {
                status = 12;
            }
            break;
        case 12:
            if(!(PINA & (1<<home_switch)))
            {
                h_bridge_set_percentage(0);
                status = 14;
                break;
            }
            h_bridge_set_percentage(-70);
        case 14:
            servo1_set_percentage(-50);
            servo2_set_percentage(-50);
            status = 2;
            break;

		}
	}

	return 0;
}
