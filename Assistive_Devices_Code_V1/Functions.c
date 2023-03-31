/*
 *
 *
 *
*/

/*
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define DEBOUNCE	_delay_ms(20)		                // debounce delay voor de knoppen
#define TCNT_INIT5  10000u                              // timer init
#define OCA_INIT5    ((TCNT_INIT5) + 25000u)            // timer compare init
#define OCB_INIT5   ((TCNT_INIT5) + 40000u)

static int buzz = 0;
static int knipper = 0;

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
*/
