#ifndef F_CPU
#define F_CPU 16000000ul
#endif

#include "h_bridge.h"
#include "servo.h"

#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>

#define DEBOUNCE	_delay_ms(20)		                // debounce delay voor de knoppen
#define TCNT_INIT5  10000u                              // timer init
#define OCA_INIT5    ((TCNT_INIT5) + 25000u)             // timer compare init
#define OCB_INIT5   ((TCNT_INIT5) + 40000u)

//schakelaars
#define home_switch         PA0
#define top_switch          PA1
#define middel_switch       PA5

//bediningpaneel
#define home_knop           PF0			// pin A0
#define opbouw_knop         PF1			// pin A1
#define afbouw_knop         PF2			// pin A2
#define volgende_steiger    PF3			// pin A3
#define vorige_steiger      PF5			// pin A5
#define nood_knop	        PF4			// pin A4

int buzz = 0;
int knipper = 0;

void buzzer(signed char power)                          // buzzer function
{                                                       // als je een '1' meegeeft aan de functie gaat de buzzer aan
    if (power == 1)                                     // als je een '0' meegeeft aan de functie gaat de buzzer uit
    {
        TIMSK5 |= (1<<OCIE5B);
    }
    else if (power == 0)
    {
        TIMSK5 |= (0<<OCIE5B);
        PORTD &= ~(1<<PD0);                             // buzzer uit
    }
}

void knipperLichten(signed char power)                  // knipperlicht functie met interrupt op timer 1
{                                                       // als je een '1' meegeeft aan de functie gaan de lichten aan
    if (power == 1)                                     // als je een '0' meegeest aan de functie gaan de lichten uit
    {
        TIMSK5 = (1<<OCIE5A) | (1<<TOIE5);	            // eneble interrupt overflow en compare A en B
    }
    else if (power == 0)
    {
        TIMSK5 = 0x00;                                  // disable timer 1
        PORTK &= ~(1<<PK7) & ~(1<<PK6);                 // knipperlichten allebei uit
    }
}

static void init_timer5(void)                           // timer 5 plus interrupt enable
{
	TCCR5A = 0;
	TCCR5B = (1<<CS52) | (0<<CS51) | (0<<CS50);         // set prescale van 256
	TCNT5 = TCNT_INIT5;
	OCR5A = OCA_INIT5;
	OCR5B = OCB_INIT5;
	TIMSK5 = (1<<OCIE5A) | (1<<OCIE5B) | (1<<TOIE5);	// eneble interrupt overflow en compare A en B
}

void init(void)
{
    init_timer5();                                      // init timer 5
	init_h_bridge();									// init h brug
	init_servo();										// init servo
	sei();											    // eneble global interrupts
}

int main(void)
{
	init();				// run init function

	// output
	DDRK |= (1<<PK7) | (1<<PK6);                        // knipperlichten pin A15 en A14
	PORTK &= ~(1<<PK7) & ~(1<<PK6);                     // knipperlichten pin A15 en A14 init uit
	DDRD |= (1<<PD0);                                   // buzzer
	PORTD &= ~(1<<PD0);                                 // buzzer init uit
	DDRF |= (1<<PF7);                                   // ledje in de noodknop
	PORTF |= (1<<PF7);                                  // ledje in de noodknop init uit

	//bedingingpaneel
	PORTF |= (1<<home_knop) | (1<<opbouw_knop) | (1<<afbouw_knop) | (1<<volgende_steiger) | (1<<vorige_steiger);   		// eneble alle knoppen voor input
	DDRF |= (1<<home_knop) | (1<<opbouw_knop) | (1<<afbouw_knop) | (1<<volgende_steiger) | (1<<vorige_steiger);		// eneble interne pullup resistor

	//schakelaars
	PORTA &= (1<<home_switch);
	PORTA &= (1<<top_switch);
	PORTA &= (1<<middel_switch);
	int status = 0;						// status voor de switch case statement

    knipperLichten(0);


	while (1)
	{
        if (buzz >= 8)
        {
            buzzer(0);
            if (TIMSK5 == 0x00)
            {
                buzz = 0;
            }
        }
        if (knipper >= 15)
        {
            knipperLichten(0);
            knipper = 0;
        }
        if ((PINF & (1<<PF4)) == 0)                 // nood stop indrukken
        {
            DEBOUNCE;
            h_bridge_set_percentage(0);
            h_bridge_set_percentage2(0);
            buzzer(0);
            knipperLichten(0);
            PORTF &= ~(1<<PF7);                     // als de noodstop is ingedrukt brandt het ledje
            status = 0;
            if ((PINF & (1<<PF4)) != 0)
            {
                DEBOUNCE;
                PORTF |= (1<<PF7);                  // als de noodstop weer wordt losgelaten gaat het ledje uit
            }
        }
		switch(status)
		{
        case 0:
            if(!(PINA & (1<<home_switch)))
            {
                DEBOUNCE;
                status = 2;
                break;
            }
            if (PINA &(1<<home_switch))
            {
                DEBOUNCE;
                if(!(PINF & (1<<home_knop)))
                {
                    DEBOUNCE;
                    knipperLichten(1);
                    buzzer(1);
                    status = 2;
                    break;
                }
            }
            break;
        case 1:
            if(!(PINA & (1<<home_switch)))
            {
                DEBOUNCE;
                status = 0;
                break;
            }
            h_bridge_set_percentage(-70);
            h_bridge_set_percentage2(-70);
            break;
        case 2:
            if(!(PINF & (1<<opbouw_knop)))
            {
                DEBOUNCE;
                knipperLichten(0);
                status = 3;
                break;
            }
            if(!(PINF & (1<<afbouw_knop)))
            {
                DEBOUNCE;
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
                DEBOUNCE;
                h_bridge_set_percentage(0);
                status = 7;
                break;
            }
            h_bridge_set_percentage(70);
            h_bridge_set_percentage2(70);
            break;
        case 7:
            if(!(PINF & (1<<volgende_steiger)))
            {
                DEBOUNCE;
                status = 9;
            }
            break;
        case 9:
            if(!(PINA & (1<<middel_switch)))
            {
                DEBOUNCE;
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
                DEBOUNCE;
                h_bridge_set_percentage(0);
                status = 2;
                break;
            }
            h_bridge_set_percentage(-70);
            break;
        case 4:
            if (!(PINA &(1<<middel_switch)))
            {
                DEBOUNCE;
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
                DEBOUNCE;
                h_bridge_set_percentage(0);
                status = 10;
                break;
            }
            h_bridge_set_percentage(70);
            break;
        case 10:
            if(!(PINF &(1<<vorige_steiger)))
            {
                DEBOUNCE;
                status = 12;
            }
            break;
        case 12:
            if(!(PINA & (1<<home_switch)))
            {
                DEBOUNCE;
                h_bridge_set_percentage(0);
                status = 14;
                break;
            }
            h_bridge_set_percentage(-70);
	    break;
        case 14:
            servo1_set_percentage(-50);
            servo2_set_percentage(-50);
            status = 2;
            break;

		}
	}

	return 0;
}

// ---- interrupts ---- //
ISR(TIMER5_COMPA_vect)          // timer 1 interrupt compare voor knipperlichten
{
	PORTK |= (1<<PK7);          // ledjes knipperen
	PORTK &= ~(1<<PK6);
	knipper++;
}

ISR(TIMER5_COMPB_vect)          // timer 1 interrupt compare voor knipperlichten
{
    PORTD |= (1<<PD0);                              // buzzer aan
    buzz++;
}

ISR(TIMER5_OVF_vect)            // timer 1 interrupt overflow voor knipperlichten
{
	TCNT5 = TCNT_INIT5;          // re-init timer 1
    PORTK |= (1<<PK6);          // ledjes knipperen
	PORTK &= ~(1<<PK7);
    PORTD &= ~(1<<PD0);                             // buzzer uit
}
