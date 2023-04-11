#include "h_bridge.h"
#include "servo.h"
#include "Library.h"

#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>

int buzz = 0;

void init(void)
{
    init_timer5();                                      // init timer 5
	init_h_bridge();									// init h brug
	init_servo();                                       // init Servo
	init_LCDenBluetooth();                              // init LCD en Bluetooth
	init_Registers();                                   // init registers
	sei();											    // enable global interrupts
}

int main(void)
{
	init();				                                // run init function

	int status = 0;						                // status voor de switch case statement

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
        if (((PINF & (1<<PF4)) == 0) || (Bluetooth_Getal == 6))//((PINF & (1<<PF4)) == 0)                // nood stop indrukken
        {
            DEBOUNCE;
            PORTF &= ~(1<<PF7);                     // als de noodstop is ingedrukt brandt het ledje
            buzzer(1);
            Bluetooth_Getal = 20;
            status = 0;
        }
		switch(status)
		{
        case 0: // nood_toestand

            knipperLichten(1);
            h_bridge_set_percentage(0);
            h_bridge_set_percentage2(0);

            LCD_Scherm_Leeg();
            LCD_Naar_Locatie(1, 1);
            Stuur_LCD_String("Noodtoestand");

            if (((PINF & (1<<PF4)) != 0) && (Bluetooth_Getal == 7))//((PINF & (1<<PF4)) != 0)     //Noodstop loslaten
            {
                DEBOUNCE;
                PORTF |= (1<<PF7);
                Bluetooth_Getal = 20;                  // als de noodstop weer wordt losgelaten gaat het ledje uit
                status = 1;
            }
            break;

        case 1: // init_state
            knipperLichten(0);          // uit
            set_servo_direction(0);     // ingeklapt
            h_bridge_set_percentage(0);
            h_bridge_set_percentage2(0);

            LCD_Scherm_Leeg();
           	LCD_Naar_Locatie(1, 1);
	    	Stuur_LCD_String("Opstarten");

            if (((PINF & (1<<in_uit_klappen)) == 0) || (Bluetooth_Getal == 8))//((PINF & (1<<in_uit_klappen)) == 0)
            {
                DEBOUNCE;
                Bluetooth_Getal = 20;
                status = 2;
            }
            break;

        case 2: // wacht
            h_bridge_set_percentage(80);    // kleine motor
            knipperLichten(1);
            buzzer(1);

            LCD_Scherm_Leeg();
           	LCD_Naar_Locatie(1, 1);
	    	Stuur_LCD_String("Uitklappen");

            if ((PINA & (1<<uitgeklapt_switch)) == 0)
            {
                DEBOUNCE;
                status = 3;
            }
            break;

        case 3: // stijger_plaatsen
            h_bridge_set_percentage(0);
            knipperLichten(0);

            LCD_Scherm_Leeg();
           	LCD_Naar_Locatie(1, 1);
	    	Stuur_LCD_String("Plaats steiger");

            if (((PINF & (1<<in_uit_klappen)) == 0) || (Bluetooth_Getal == 8)) //((PINF & (1<<in_uit_klappen)) == 0)        //Weer inklappen
            {
                DEBOUNCE;
                status = 12;
            }
            if (((PINF & (1<<opbouw_knop)) == 0) || (Bluetooth_Getal == 3))//((PINF & (1<<opbouw_knop)) == 0)       //Opbouwen
            {
                DEBOUNCE;
                status = 4;
            }
            break;

        case 4: // opbouwen
            set_servo_direction(1); // uitgeklapt
            buzzer(1); // aan
            knipperLichten(1); // aan

            LCD_Scherm_Leeg();
           	LCD_Naar_Locatie(1, 1);
	    	Stuur_LCD_String("Opbouwen");

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
            knipperLichten(0); // uit

            LCD_Scherm_Leeg();
           	LCD_Naar_Locatie(1, 2);
	    	Stuur_LCD_String("Plaats volgende");
            LCD_Naar_Locatie(1, 1);
	    	Stuur_LCD_String("steiger");

            if (((PINF & (1<<neerzetten_knop)) == 0) || (Bluetooth_Getal == 5))//((PINF & (1<<neerzetten_knop)) == 0)
            {
                DEBOUNCE;
                status = 6;
            }
            break;

        case 6: // neerzetten (opbouwen)
            h_bridge_set_percentage2(-50);
            buzzer(1); // aan
            knipperLichten(1); // aan

            LCD_Scherm_Leeg();
            LCD_Naar_Locatie(1, 1);
	    	Stuur_LCD_String("Neerzetten");

            if((PINA & (1<<middel_switch)) == 0)
            {
                DEBOUNCE;
                status = 7;
            }
            break;

        case 7: // stijger_opgestapeld
            h_bridge_set_percentage2(0);
            set_servo_direction(0); // ingeklapt
            knipperLichten(0); // uit

            LCD_Scherm_Leeg();
            LCD_Naar_Locatie(1, 1);
	    	Stuur_LCD_String("Plaats volgende");

            if (((PINF & (1<<opbouw_knop)) == 0) || (Bluetooth_Getal == 3))//((PINF & (1<<opbouw_knop)) == 0)       //Opbouwen
            {
                DEBOUNCE;
                status = 8;
            }
            if (((PINF & (1<<afbouw_knop)) == 0) || (Bluetooth_Getal == 4))//((PINF & (1<<afbouw_knop)) == 0)       /Afbouwen
            {
                DEBOUNCE;
                status = 9;
            }
            break;

        case 8: // door_bouwen
            h_bridge_set_percentage2(80);
            knipperLichten(1); // aan
            buzzer(1); // aan

            LCD_Scherm_Leeg();
            LCD_Naar_Locatie(1, 1);
	    	Stuur_LCD_String("Door bouwen");

            if ((PINA & (1<<home_switch)) == 0)
            {
                DEBOUNCE;
                status = 4;
            }
            break;

        case 9: // afbouwen
            buzzer(1); // aan
            knipperLichten(1); // aan
            set_servo_direction(1); // uitgeklapt

            LCD_Scherm_Leeg();
            LCD_Naar_Locatie(1, 1);
	    	Stuur_LCD_String("Afbouwen");

            // moet pas na het uitklappen van de servo's
            h_bridge_set_percentage2(20);
            if ((PINA & (1<<top_switch)) == 0)
            {
                DEBOUNCE;
                status = 10; // steiger weghalen
            }
           break;

        case 10: // stijger_weghalen
            knipperLichten(0); // uit

            LCD_Scherm_Leeg();
            LCD_Naar_Locatie(1, 1);
	    	Stuur_LCD_String("Steiger weghalen");

            if (((PINF & (1<<neerzetten_knop)) == 0) || (Bluetooth_Getal == 5))//((PINF & (1<<neerzetten_knop)) == 0)
            {
                DEBOUNCE;
                status = 11;
            }
            break;

        case 11: // neerzetten (afbouwen)
            buzzer(1); // aan
            knipperLichten(1); // aan

            /*
            MOTOR AAN??
            h_bridge_set_percentage2(-20);
            */

            LCD_Scherm_Leeg();
            LCD_Naar_Locatie(1, 1);
	    	Stuur_LCD_String("Neerzetten");

            if ((PINA & (1<<home_switch)) == 0)
            {
                DEBOUNCE;
                status = 3;
            }
            break;
        case 12: //inklappen
            knipperLichten(1); // aan
            buzzer(1); // aan

            /*
            Servo's in??

            set_servo_direction(0); // ingeklapt
            _delay_ms(500);
            */

            h_bridge_set_percentage(-80);    // keine motor

            LCD_Scherm_Leeg();
           	LCD_Naar_Locatie(1, 1);
	    	Stuur_LCD_String("Inklappen");

            if ((PINA & (1<<ingeklapt_switch)) == 0)
            {
                DEBOUNCE;
                status = 1;
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
