#ifndef F_CPU
#define F_CPU 16000000ul
#endif

#include "h_bridge.h"
#include "servo.h"
#include "functions.c"

#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>

#define DEBOUNCE	_delay_ms(20)		                // debounce delay voor de knoppen
#define TCNT_INIT5  10000u                              // timer init
#define OCA_INIT5    ((TCNT_INIT5) + 25000u)            // timer compare init
#define OCB_INIT5   ((TCNT_INIT5) + 40000u)

//schakelaars
#define home_switch         PA1         // pin 23
#define top_switch          PA3         // pin 25
#define middel_switch       PA5         // pin 27
#define uitgeklapt_switch   PA7         // pin 29
#define ingeklapt_switch    PC6         // pin 31    let op port C!

//bediningpaneel
#define opbouw_knop         PF0			// pin A0
#define afbouw_knop         PF1			// pin A1
#define neerzetten_knop     PF2			// pin A2
#define in_uit_klappen      PF3         // pin A3
#define nood_knop	        PF4			// pin A4

int buzz = 0;

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

static void init_timer5(void)                            // timer 5 plus interrupt enable
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
	PORTF |= (1<<opbouw_knop) | (1<<afbouw_knop) | (1<<neerzetten_knop) | (1<<nood_knop);   		// eneble alle knoppen voor input
	DDRF |= (1<<opbouw_knop) | (1<<afbouw_knop) | (1<<neerzetten_knop) | (1<<nood_knop);		// eneble interne pullup resistor

	//schakelaars
	PORTA &= (1<<home_switch);
	PORTA &= (1<<top_switch);
	PORTA &= (1<<middel_switch);
	int status = 0;						// status voor de switch case statement

    knipperLichten(0);
    h_bridge_set_percentage(0);
    h_bridge_set_percentage2(0);

	while (1)
	{
        if (buzz >= 8)                              // buzzer gaat 8 keer aan en uit door timer 5
        {
            buzzer(0);                              // na 8 keer stopt de buzzer
            if (TIMSK5 == 0x00)                     // als timer 5 uit staat
            {
                buzz = 0;                           // wordt de buzzer gereset en kan opnieuw aangezet worden
            }
        }
//        if (knipper >= 15)
//        {
//            knipperLichten(0);
//            knipper = 0;
//        }
        if ((PINF & (1<<PF4)) == 0)                 // nood stop indrukken
        {
            DEBOUNCE;
            PORTF &= ~(1<<PF7);                     // als de noodstop is ingedrukt brandt het ledje
            buzzer(1);
            status = 0;
        }
		switch(status)
		{
        case 0: // nood_toestand

            knipperLichten(1);
            h_bridge_set_percentage(0);
            h_bridge_set_percentage2(0);

            /// LCD (nood situatie)

            if ((PINF & (1<<PF4)) != 0)
            {
                DEBOUNCE;
                PORTF |= (1<<PF7);                  // als de noodstop weer wordt losgelaten gaat het ledje uit
                status = 1;
            }
            break;

        case 1: // init_state
            knipperLichten(0);
            servo1_set_percentage(0);    // ingeklapt
            servo2_set_percentage(0);    // ingeklapt
            h_bridge_set_percentage(0);
            h_bridge_set_percentage2(0);

            /// LCD (start)

            if ((PINF & (1<<in_uit_klappen)) == 0)
            {
                DEBOUNCE;
                status = 2;
            }
            break;

        case 2: // wacht
            h_bridge_set_percentage(80);    // keine motor
            knipperLichten(1);
            buzzer(1);

            /// LCD (omhoog)

            if ((PINA & (1<<uitgeklapt_switch)) == 0)
            {
                DEBOUNCE;
                status = 3;
            }
            break;

        case 3: // stijger_plaatsen
            h_bridge_set_percentage(0);
            knipperLichten(0);

            /// LCD (plaats stijgerdeel)

            if ((PINF & (1<<in_uit_klappen)) == 0)
            {
                DEBOUNCE;
                // case inklappen
            }
            if ((PINF & (1<<opbouw_knop)) == 0)
            {
                DEBOUNCE;
                status = 4;
            }
            break;

        case 4: // opbouwen
            servo1_set_percentage(90);      // hier moet nog gekeken worden wat de juiste waarde is
            servo2_set_percentage(90);      // ze moeten hier worden uitgeklapt
            buzzer(1);
            knipperLichten(1);

            /// LCD (opbouw)

            _delay_ms(500); // om te zorgen dat de servo's uit zijn voor de motor aan gaat

            h_bridge_set_percentage2(80);

            if ((PINA & (1<<top_switch)) == 0)
            {
                DEBOUNCE;
                status = 5;
            }
            break;

        case 5: // volgende_stijger
            h_bridge_set_percentage2(0);
            knipperLichten(0);

            /// LCD (plaats volgend stijgerdeer)

            if ((PINF & (1<<neerzetten_knop)) == 0)
            {
                DEBOUNCE;
                status = 6;
            }
            break;

        case 6: // neerzetten (opbouwen)
            h_bridge_set_percentage2(-50);
            buzzer(1);
            knipperLichten(1);

            /// LCD (neerzetten)

            if ((PINA & (1<<middel_switch)) == 0)
            {
                DEBOUNCE;
                status = 7;
            }
            break;

        case 7: // stijger_opgestapeld
            h_bridge_set_percentage2(0);
            servo1_set_percentage(0);       // moet nog naar gekeken worden
            servo2_set_percentage(0);       // servo's moeten ingeklapt
            knipperLichten(0);

            /// LDC (stijger opgestapeld)

            if ((PINF & (1<<opbouw_knop)) == 0)
            {
                DEBOUNCE;
                status = 8;
            }
            if ((PINF & (1<<afbouw_knop)) == 0)
            {
                DEBOUNCE;
                status = 9;
            }
            break;

        case 8: // door_bouwen
            h_bridge_set_percentage2(80);
            knipperLichten(1);
            buzzer(1);

            /// LCD (door bouwen)

            if ((PINA & (1<<home_switch)) == 0)
            {
                DEBOUNCE;
                status = 4;
            }
            break;

        case 9: // afbouwen
            buzzer(1);
            knipperLichten(1);
            servo1_set_percentage(50);      // moet nog naar gekeken worden
            servo2_set_percentage(50);      // moet worden uitgeklapt

            /// LCD (afbouwen)

            h_bridge_set_percentage2(20);
            if ((PINA & (1<<top_switch)) == 0)
            {
                DEBOUNCE;
                status = 10;
            }
           break;

        case 10: // stijger_weghalen
            knipperLichten(0);

            /// LCD (stijger weghalen)

            if ((PINF & (1<<neerzetten_knop)) == 0)
            {
                DEBOUNCE;
                status = 11;
            }
            break;

        case 11: // neerzetten (afbouwen)
            buzzer(1);
            knipperLichten(1);

            /// LCD (neerzetten)

            if ((PINA & (1<<home_switch)) == 0)
            {
                DEBOUNCE;
                status = 3;
            }
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
