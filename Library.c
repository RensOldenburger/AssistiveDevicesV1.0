#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "Library.h"

char LCD_Pos_Eerste_Vakje[2] = {0, 64};

int Bluetooth_Getal = 0;//BLUETOOTH GETAL

void Check_LCD_isbezig(void);
void LCD_Aan_Uit(void);
void Stuur_LCD_Character(unsigned char LCD_character);

ISR(USART0_RX_vect)
{
    Bluetooth_Getal = UDR0;
}

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

void set_servo_direction(signed char direction)
{
    if (direction == 1) // uitgeklapt
    {
        servo1_set_percentage(100);
        servo2_set_percentage(-100);
    }
    if (direction == 0) // ingeklapt
    {
        servo1_set_percentage(-100);
        servo2_set_percentage(100);
    }
}

void init_timer5(void)                            // timer 5 plus interrupt enable
{
	TCCR5A = 0;
	TCCR5B = (1<<CS52) | (0<<CS51) | (0<<CS50);         // set prescale van 256
	TCNT5 = TCNT_INIT5;
	OCR5A = OCA_INIT5;
	OCR5B = OCB_INIT5;
	TIMSK5 = (1<<OCIE5A) | (1<<OCIE5B) | (1<<TOIE5);	// eneble interrupt overflow en compare A en B
}

void init_Registers(void)
{
    // output
	DDRK |= (1<<PK7) | (1<<PK6);                        // knipperlichten pin A15 en A14
	PORTK &= ~(1<<PK7) & ~(1<<PK6);                     // knipperlichten pin A15 en A14 init uit
	DDRD |= (1<<PD0);                                   // buzzer
	PORTD &= ~(1<<PD0);                                 // buzzer init uit
	DDRF |= (1<<PF7);                                   // ledje in de noodknop
	PORTF |= (1<<PF7);                                  // ledje in de noodknop init uit

	//bedingingpaneel
	PORTF |= (1<<opbouw_knop) | (1<<afbouw_knop) | (1<<neerzetten_knop) | (1<<nood_knop) | (1<<in_uit_klappen);   		// eneble alle knoppen voor input
	DDRF |= (1<<opbouw_knop) | (1<<afbouw_knop) | (1<<neerzetten_knop) | (1<<nood_knop) | (1<<in_uit_klappen);		// eneble interne pullup resistor

	//schakelaars
	PORTA &= (1<<home_switch);
	PORTA &= (1<<top_switch);
	PORTA &= (1<<middel_switch);
}

void init_LCDenBluetooth(void)
{
    //LCD scherm
    DataDir_LCDControl |= (1 << LCDEnable) | (1 << ReadWrite) | (1 << LCDRS);//Data directory register goed zetten
    _delay_ms(15);//delay moet volgens datasheet

    LCD_Scherm_Leeg();
    Stuur_LCD_Opdracht(0x38);//opdracht voor LCD in 8bit mode
    _delay_us(50);//delay moet volgens datasheet
    Stuur_LCD_Opdracht(0b00001100);//scherm een beetje aanpassen
    _delay_us(50);//delay moet volgens datasheet

    //Bluetooth
    UBRR0H = (BRC >> 8);
    UBRR0L = BRC;

    UCSR0B = (1 << RXEN0) |(1 << RXCIE0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    sei();
}

void Check_LCD_isbezig(void)//wachten tot LCD informatie kan opnemen
{
    DataDir_LCDpoort = 0;
    LCDControl |= (1 << ReadWrite);
    LCDControl &= ~(1 << LCDRS);

    while(LCDpoort >= 0x80)
    {
        LCD_Aan_Uit();
    }

    DataDir_LCDpoort = 0xFF;
}

void LCD_Aan_Uit(void)//LCD(enable) snel aan en uit zetten
{
    LCDControl |= 1 << (LCDEnable);
    _delay_us(50);
    LCDControl &= ~(1 << LCDEnable);
}

void Stuur_LCD_Opdracht(unsigned char LCD_opdracht)//opdracht naar de LCD sturen
{
    Check_LCD_isbezig();
    LCDpoort = LCD_opdracht;
    LCDControl &= ~((1 << ReadWrite) | (1 << LCDRS));
    LCD_Aan_Uit();
    LCDpoort = 0;
}

void Stuur_LCD_Character(unsigned char LCD_character)//character naar de LCD sturen
{
    Check_LCD_isbezig();
    LCDpoort = LCD_character;
    LCDControl &= ~(1 << ReadWrite);
    LCDControl |= (1 << LCDRS);
    LCD_Aan_Uit();
    LCDpoort = 0;
    _delay_ms(10);
}

void Stuur_LCD_String(char *stringcharacters)//dit is zodat je characters in string kunnen sturen
{
    while(*stringcharacters > 0)
    {
        Stuur_LCD_Character(*stringcharacters++);
    }
}

void LCD_Naar_Locatie(uint8_t x, uint8_t y)//opdracht voor cursor verplaatsing maken
{
    Stuur_LCD_Opdracht(0x80 + LCD_Pos_Eerste_Vakje[y-1] + (x-1));
}

void LCD_Scherm_Leeg(void)//Maakt het scherm leeg
{
    Stuur_LCD_Opdracht(0x01);//Leeg scherm 0x01
    _delay_ms(2);//altijd 2ms delay na scherm leegmaken
}
