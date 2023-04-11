#ifndef LIBRARY_H_INCLUDED
#define LIBRARY_H_INCLUDED

#include <stdint.h>

//LCD scherm
#define LCDpoort            PORTC
#define DataDir_LCDpoort    DDRC
#define LCDControl          PORTD
#define DataDir_LCDControl  DDRD
#define LCDEnable           2
#define ReadWrite           1
#define LCDRS               0

//Bluetooth
#define F_CPU   16000000
#define BUAD    9600
#define BRC     ((F_CPU/16/BUAD)-1)//buad rate calculator

//Debounce en timers
#define DEBOUNCE	_delay_ms(20)		                // debounce delay voor de knoppen
#define TCNT_INIT5  10000u                              // timer init
#define OCA_INIT5    ((TCNT_INIT5) + 25000u)            // timer compare init
#define OCB_INIT5   ((TCNT_INIT5) + 40000u)

//Schakelaars
#define home_switch         PA1         // pin 23
#define top_switch          PA3         // pin 25
#define middel_switch       PA5         // pin 27
#define uitgeklapt_switch   PA7         // pin 29
#define ingeklapt_switch    PC6         // pin 31    let op port C!

//Bediningpaneel
#define opbouw_knop         PF0			// pin A0
#define afbouw_knop         PF1			// pin A1
#define neerzetten_knop     PF2			// pin A2
#define in_uit_klappen      PF3         // pin A3
#define nood_knop	        PF4			// pin A4

//Variabelen
//extern int buzz = 0;
extern int Bluetooth_Getal;
extern int Btgetallen[8];//BEETJE ONNODIG MISSCHIEN NIET HELEMAAL

//Functies
void buzzer(signed char power);
void knipperLichten(signed char power);
void set_servo_direction(signed char direction);
void init_timer5(void);
void init_Registers(void);

//LCD scherm
void init_LCDenBluetooth(void);
void Stuur_LCD_Opdracht(unsigned char LCD_opdracht);
void Stuur_LCD_String(char *stringcharacters);
void LCD_Naar_Locatie(uint8_t x, uint8_t y);
void LCD_Scherm_Leeg(void);

#endif // LIBRARY_H_INCLUDED
