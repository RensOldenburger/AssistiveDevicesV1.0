#include "h_bridge.h"
#include "servo.h"
#include "Library.h"

#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>

int State_verander = 1;

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
    DDRB |= (1 << PB7);
    PORTB |= (1 << PB7);
	init();				                                // run init function

	int status = 1;						                // status voor de switch case statement

    knipperLichten(0);
    h_bridge_set_percentage(0);
    h_bridge_set_percentage2(0);

	while (1)
	{
        if ((PINF & (1<<PF4)) == 0)//((PINF & (1<<PF4)) == 0)      // noodstop indrukken
        {
            DEBOUNCE;
            PORTF &= ~(1<<PF7);                     // als de noodstop is ingedrukt brandt het ledje
            status = 0;
        }
        if (Bluetooth_Getal == 6)               // noodstop app indrukken
        {
            DEBOUNCE;
            PORTF &= ~(1<<PF7);                     // als de noodstop is ingedrukt brandt het ledje
            Bluetooth_Getal = 20;
            status = 99;
            State_verander = status;
        }
		switch(status)
		{
        case 0: // nood_toestand
            buzzer(1);
            knipperLichten(1);
            h_bridge_set_percentage(0);
            h_bridge_set_percentage2(0);
            if((State_verander != status) && (State_verander != 98))
            {
                LCD_Scherm_Leeg();
                LCD_Naar_Locatie(1, 1);
                Stuur_LCD_String("!!Noodtoestand!!");
                State_verander = 98;
            }
            if ((PINF & (1<<PF4)) != 0)     //Noodstop loslaten
            {
                DEBOUNCE;
                PORTF |= (1<<PF7);
                Bluetooth_Getal = 20;                  // als de noodstop weer wordt losgelaten gaat het ledje uit
                status = 666;
                State_verander = status;
            }
            break;
        case 99: // nood_toestand
            buzzer(1);
            knipperLichten(1);
            h_bridge_set_percentage(0);
            h_bridge_set_percentage2(0);
            if(State_verander == status)
            {
                State_verander = status;
            }
            if(State_verander == status)
            {
                LCD_Scherm_Leeg();
                LCD_Naar_Locatie(1, 1);
                Stuur_LCD_String("!!Noodtoestand!!");
                State_verander = 100;
            }

            if ((Bluetooth_Getal == 7) && ((PINF & (1<<PF4)) != 0))     //Noodstop loslaten
            {
                DEBOUNCE;
                PORTF |= (1<<PF7);
                Bluetooth_Getal = 20;                  // als de noodstop weer wordt losgelaten gaat het ledje uit
                status = 666;
                State_verander = status;
            }
            break;

        case 666: //Keuze case na Noodtoestand
            buzzer(0);
            knipperLichten(0);
            h_bridge_set_percentage(0);
            h_bridge_set_percentage2(0);
            if(State_verander == status)
            {
                LCD_Scherm_Leeg();
                LCD_Naar_Locatie(1, 1);
                Stuur_LCD_String("Kies volgende");
                LCD_Naar_Locatie(6, 2);
                Stuur_LCD_String("actie");
                State_verander = 100;
            }

            if (((PINF & (1<<opbouw_knop)) == 0) || (Bluetooth_Getal == 3))//Opbouwen
            {
                DEBOUNCE;
                Bluetooth_Getal = 20;
                status = 4;
                State_verander = status;
            }
            if (((PINF & (1<<afbouw_knop)) == 0) || (Bluetooth_Getal == 4))//Afbouwen
            {
                DEBOUNCE;
                Bluetooth_Getal = 20;
                status = 9;
                State_verander = status;
            }
            if (((PINF & (1<<neerzetten_knop)) == 0) || (Bluetooth_Getal == 5))//Neerzetten
            {
                DEBOUNCE;
                Bluetooth_Getal = 20;
                status = 6;
                State_verander = status;
            }
            if ((((PINF & (1<<in_uit_klappen_knop)) == 0) || (Bluetooth_Getal == 8)) && ((PINA & (1<<ingeklapt_switch)) == 0))//Uitklappen maar alleen als die ingeklapt is
            {
                DEBOUNCE;
                Bluetooth_Getal = 20;
                status = 2;
                State_verander = status;
            }
            if ((((PINF & (1<<in_uit_klappen_knop)) == 0) || (Bluetooth_Getal == 8)) && ((PINA & (1<<uitgeklapt_switch)) == 0))//Inklappen maar alleen als die uitgeklapt is
            {
                DEBOUNCE;
                Bluetooth_Getal = 20;
                status = 12;
                State_verander = status;
            }
            break;

        case 1: //init_state
            buzzer(0);
            knipperLichten(0);
            set_servo_direction(0);    // ingeklapt
            h_bridge_set_percentage(0);
            h_bridge_set_percentage2(0);

            if(State_verander == status)
            {
                LCD_Scherm_Leeg();
                LCD_Naar_Locatie(4, 1);
                Stuur_LCD_String("Opstarten");
                State_verander = 100;
            }

            if (((PINF & (1<<in_uit_klappen_knop)) == 0) || (Bluetooth_Getal == 8))//((PINF & (1<<in_uit_klappen)) == 0)
            {
                DEBOUNCE;
                Bluetooth_Getal = 20;
                status = 2;
                State_verander = status;
            }
            break;

        case 2: // Uitklappen
            h_bridge_set_percentage(80);    // kleine motor
            knipperLichten(1);
            buzzer(1);

            if(State_verander == status)
            {
                LCD_Scherm_Leeg();
                LCD_Naar_Locatie(4, 1);
                Stuur_LCD_String("Uitklappen");
                State_verander = 100;
            }

            if ((PINA & (1<<uitgeklapt_switch)) == 0)
            {
                DEBOUNCE;
                status = 3;
                State_verander = status;
            }
            break;

        case 3: // Uitgeklapt
            h_bridge_set_percentage(0);
            knipperLichten(0);
            buzzer(0);

            if(State_verander == status)
            {
                LCD_Scherm_Leeg();
                LCD_Naar_Locatie(1, 1);
                Stuur_LCD_String("Plaats steiger");
                LCD_Naar_Locatie(1, 2);
                Stuur_LCD_String("of klap weer in");
                State_verander = 100;
            }

            if (((PINF & (1<<in_uit_klappen_knop)) == 0) || (Bluetooth_Getal == 8))        //Weer inklappen
            {
                DEBOUNCE;
                Bluetooth_Getal = 20;
                status = 12;
                State_verander = status;
            }
            if (((PINF & (1<<opbouw_knop)) == 0) || (Bluetooth_Getal == 3))      //Opbouwen
            {
                DEBOUNCE;
                Bluetooth_Getal = 20;
                status = 4;
                State_verander = status;
            }
            break;

        case 4: // opbouwen
            set_servo_direction(1); // uitgeklapt
            buzzer(1); // aan
            knipperLichten(1); // aan

            if(State_verander == status)
            {
                LCD_Scherm_Leeg();
                LCD_Naar_Locatie(4, 1);
                Stuur_LCD_String("Opbouwen");
                State_verander = 100;

                _delay_ms(500); // om te zorgen dat de servo's uit zijn voor de motor aan gaat
            }
            if ((PINA & (1<<home_switch)) != 0)
            {
                h_bridge_set_percentage2(-80);
            }
            if((PINA & (1<<home_switch)) == 0)
            {
                            h_bridge_set_percentage2(80);
            }

            if ((PINA & (1<<top_switch)) == 0)
            {
                DEBOUNCE;
                status = 5;
                State_verander = status;
            }
            break;

        case 5: // volgende_stijger
            h_bridge_set_percentage2(0);
            knipperLichten(0); // uit
            buzzer(0);

            if(State_verander == status)
            {
                LCD_Scherm_Leeg();
                LCD_Naar_Locatie(1, 1);
                Stuur_LCD_String("Plaats volgende");
                LCD_Naar_Locatie(4, 2);
                Stuur_LCD_String("steiger");
                State_verander = 100;
            }

            if (((PINF & (1<<neerzetten_knop)) == 0) || (Bluetooth_Getal == 5))//((PINF & (1<<neerzetten_knop)) == 0)
            {
                DEBOUNCE;
                Bluetooth_Getal = 20;
                status = 6;
                State_verander = status;
            }
            break;

        case 6: // neerzetten (opbouwen)
            h_bridge_set_percentage2(-50);
            buzzer(1);//aan
            knipperLichten(1);//aan

            if(State_verander == status)
            {
                LCD_Scherm_Leeg();
                LCD_Naar_Locatie(4, 1);
                Stuur_LCD_String("Neerzetten");
                State_verander = 100;
            }

            if((PINA & (1<<middel_switch)) == 0)
            {
                DEBOUNCE;
                status = 7;
                State_verander = status;
            }
            break;

        case 7: // stijger_opgestapeld
            h_bridge_set_percentage2(0);
            set_servo_direction(0);//ingeklapt
            knipperLichten(0);//uit
            buzzer(0);//aan

            if(State_verander == status)
            {
                LCD_Scherm_Leeg();
                LCD_Naar_Locatie(1, 1);
                Stuur_LCD_String("Kies doorbouwen");
                LCD_Naar_Locatie(3, 2);
                Stuur_LCD_String("of afbouwen");
                State_verander = 100;
            }

            if (((PINF & (1<<opbouw_knop)) == 0) || (Bluetooth_Getal == 3))     //Opbouwen
            {
                DEBOUNCE;
                status = 8;
                State_verander = status;
            }
            if (((PINF & (1<<afbouw_knop)) == 0) || (Bluetooth_Getal == 4))       //Afbouwen
            {
                DEBOUNCE;
                status = 9;
                State_verander = status;
            }
            break;

        case 8: // door_bouwen
            h_bridge_set_percentage2(80);
            knipperLichten(1); // aan
            buzzer(1); // aan

            if(State_verander == status)
            {
                LCD_Scherm_Leeg();
                LCD_Naar_Locatie(2, 1);
                Stuur_LCD_String("Door bouwen");
                State_verander = 100;
            }

            if ((PINA & (1<<home_switch)) == 0)
            {
                DEBOUNCE;
                status = 4;
                State_verander = status;
            }
            break;

        case 9: // afbouwen
            buzzer(1); // aan
            knipperLichten(1); // aan
            set_servo_direction(1); // uitgeklapt

            if(State_verander == status)
            {
                LCD_Scherm_Leeg();
                LCD_Naar_Locatie(4, 1);
                Stuur_LCD_String("Afbouwen");
                State_verander = 100;

                _delay_ms(500); // om te zorgen dat de servo's uit zijn voor de motor aan gaat
            }

            h_bridge_set_percentage2(20);
            if ((PINA & (1<<top_switch)) == 0)
            {
                DEBOUNCE;
                status = 10; // steiger weghalen
                State_verander = status;
            }
           break;

        case 10: // stijger_weghalen
            knipperLichten(0); // uit

            if(State_verander == status)
            {
                LCD_Scherm_Leeg();
                LCD_Naar_Locatie(1, 1);
                Stuur_LCD_String("Steiger weghalen");
                State_verander = 100;
            }

            if (((PINF & (1<<neerzetten_knop)) == 0) || (Bluetooth_Getal == 5))//((PINF & (1<<neerzetten_knop)) == 0)
            {
                DEBOUNCE;
                status = 11;
                State_verander = status;
            }
            break;

        case 11: // neerzetten (afbouwen)
            buzzer(1); // aan
            knipperLichten(1); // aan

            h_bridge_set_percentage2(-20);

            if(State_verander == status)
            {
                LCD_Scherm_Leeg();
                LCD_Naar_Locatie(4, 1);
                Stuur_LCD_String("Neerzetten");
                State_verander = 100;
            }

            if ((PINA & (1<<home_switch)) == 0)
            {
                DEBOUNCE;
                status = 3;
                State_verander = status;
            }
            break;
        case 12: //inklappen
            knipperLichten(1); // aan
            buzzer(1); // aan

            set_servo_direction(0); // ingeklapt

            if(State_verander == status)
            {
                LCD_Scherm_Leeg();
                LCD_Naar_Locatie(4, 1);
                Stuur_LCD_String("Inklappen");
                State_verander = 100;

                _delay_ms(500); // om te zorgen dat de servo's uit zijn voor de motor aan gaat
            }

            h_bridge_set_percentage(-80);    // keine motor

            if ((PINA & (1<<ingeklapt_switch)) == 0)
            {
                DEBOUNCE;
                status = 1;
                State_verander = status;
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
}

ISR(TIMER5_OVF_vect)            // timer 1 interrupt overflow voor knipperlichten
{
	TCNT5 = TCNT_INIT5;          // re-init timer 1
    PORTK |= (1<<PK6);          // ledjes knipperen
	PORTK &= ~(1<<PK7);
    PORTD &= ~(1<<PD0);                             // buzzer uit
}
