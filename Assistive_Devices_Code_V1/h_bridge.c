/*
 * h_bridge.c - XvR 2020
 *
 * Use 8-bit timer. Uses interrupts in order to be able
 * to use the pins on the multifunction shield
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "h_bridge.h"

ISR(TIMER0_OVF_vect)
{
	if (OCR0A == 0 && OCR0B == 0)
	{
		PORT_RPWM &= ~(1<<PIN_RPWM);
		PORT_LPWM &= ~(1<<PIN_LPWM);
	}
	else if (OCR0A != 0)
	{
		PORT_LPWM &= ~(1<<PIN_LPWM);
		PORT_RPWM |= (1<<PIN_RPWM);
	}
	else if (OCR0B != 0)
	{
		PORT_RPWM &= ~(1<<PIN_RPWM);
		PORT_LPWM |= (1<<PIN_LPWM);
	}
}
//ISR(TIMER1_OVF_vect)
//{
//	if (OCR1A == 0 && OCR1B == 0)
//	{
//		PORT_RPWM1 &= ~(1<<PIN_RPWM1);
//		PORT_LPWM1 &= ~(1<<PIN_LPWM1);
//	}
//	else if (OCR1A != 0)
//	{
//		PORT_LPWM1 &= ~(1<<PIN_LPWM1);
//		PORT_RPWM1 |= (1<<PIN_RPWM1);
//	}
//	else if (OCR1B != 0)
//	{
//		PORT_RPWM1 &= ~(1<<PIN_RPWM1);
//		PORT_LPWM1 |= (1<<PIN_LPWM1);
//	}
//}
ISR(TIMER3_OVF_vect)
{
	if (OCR3A == 0 && OCR3B == 0)
	{
		PORT_RPWM2 &= ~(1<<PIN_RPWM2);
		PORT_LPWM2 &= ~(1<<PIN_LPWM2);
	}
	else if (OCR3A != 0)
	{
		PORT_LPWM2 &= ~(1<<PIN_LPWM2);
		PORT_RPWM2 |= (1<<PIN_RPWM2);
	}
	else if (OCR3B != 0)
	{
		PORT_RPWM2 &= ~(1<<PIN_RPWM2);
		PORT_LPWM2 |= (1<<PIN_LPWM2);
	}
}

ISR(TIMER0_COMPA_vect)
{
	if (OCR0A != 255)
	{
		PORT_RPWM &= ~(1<<PIN_RPWM);
	}
}

//ISR(TIMER1_COMPA_vect)
//{
//	if (OCR1A != 65535)
//	{
//		PORT_RPWM1 &= ~(1<<PIN_RPWM1);
//	}
//}

ISR(TIMER3_COMPA_vect)
{
	if (OCR3A != 65535)
	{
		PORT_RPWM2 &= ~(1<<PIN_RPWM2);
	}
}

ISR(TIMER0_COMPB_vect)
{
	if (OCR0B != 255)
	{
		PORT_LPWM &= ~(1<<PIN_LPWM);
	}
}

//ISR(TIMER1_COMPB_vect)
//{
//	if (OCR1B != 65535)
//	{
//		PORT_LPWM1 &= ~(1<<PIN_LPWM1);
//	}
//}

ISR(TIMER3_COMPB_vect)
{
	if (OCR3B != 65535)
	{
		PORT_LPWM2 &= ~(1<<PIN_LPWM2);
	}
}

void init_h_bridge(void)
{
	// Config pins as output
	DDR_RPWM |= (1<<PIN_RPWM);
	DDR_LPWM |= (1<<PIN_LPWM);
//	DDR_RPWM1 |= (1<<PIN_RPWM1);
//	DDR_LPWM1 |= (1<<PIN_LPWM1);
	DDR_RPWM2 |= (1<<PIN_RPWM2);
	DDR_LPWM2 |= (1<<PIN_LPWM2);

	// Output low
	PORT_RPWM &= ~(1<<PIN_RPWM);
	PORT_LPWM &= ~(1<<PIN_LPWM);
//	PORT_RPWM1 &= ~(1<<PIN_RPWM1);
//	PORT_LPWM1 &= ~(1<<PIN_LPWM1);
	PORT_RPWM2 &= ~(1<<PIN_RPWM2);
	PORT_LPWM2 &= ~(1<<PIN_LPWM2);



	// Use mode 0, clkdiv = 64
	TCCR0A = 0;
	TCCR0B = (0<<CS02) | (1<<CS01) | (1<<CS00);

	// Disable PWM output
	OCR0A = 0;
	OCR0B = 0;

	// Interrupts on OCA, OCB and OVF
	TIMSK0 = (1<<OCIE0B) | (1<<OCIE0A) | (1<<TOIE0);

	// Use mode 0, clkdiv = 64
//	TCCR1A = 0;
//	TCCR1B = (1<<CS10);
//
//	// Disable PWM output
//	OCR1A = 0;
//	OCR1B = 0;

	// Interrupts on OCA, OCB and OVF
//	TIMSK1 = (1<<OCIE1B) | (1<<OCIE1A) | (1<<TOIE1);

	TCCR3A = 0;
	TCCR3B = (1<<CS30);

	// Disable PWM output
	OCR3A = 0;
	OCR3B = 0;

	// Interrupts on OCA, OCB and OVF
	TIMSK3 = (1<<OCIE3B) | (1<<OCIE3A) | (1<<TOIE3);


	sei();
}

void h_bridge_set_percentage(signed char percentage)
{
	if (percentage >= -100 && percentage <= 100)
	{
		if (percentage >= 0)
		{
			// Disable LPWM, calculate RPWM
			OCR0B = 0;
			OCR0A = (255*percentage)/100;
		}
		else // percentage < 0
		{
			// Disable RPWM, calculate LPWM
			OCR0A = 0;
			OCR0B = (255*percentage)/-100;
		}
	}
}


void h_bridge_set_percentage1(signed char percentage)
{
	if (percentage >= -100 && percentage <= 100)
	{
		if (percentage >= 0)
		{
			// Disable LPWM, calculate RPWM
			OCR1B = 0;
			OCR1A = (65535*percentage)/100;
		}
		else // percentage < 0
		{
			// Disable RPWM, calculate LPWM
			OCR1A = 0;
			OCR1B = (65535*percentage)/-100;
		}
	}
}
void h_bridge_set_percentage2(signed char percentage)
{
	if (percentage >= -100 && percentage <= 100)
	{
		if (percentage >= 0)
		{
			// Disable LPWM, calculate RPWM
			OCR3B = 0;
			OCR3A = (65535*percentage)/100;
		}
		else // percentage < 0
		{
			// Disable RPWM, calculate LPWM
			OCR3A = 0;
			OCR3B = (65535*percentage)/-100;
		}
	}
}
