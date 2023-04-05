#ifndef LCDENBLUETOOTH_H_INCLUDED
#define LCDENBLUETOOTH_H_INCLUDED

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

extern int Bluetooth_Getal;
extern int verandergetal;//ONNODIG
extern int veranderandergetal;//ONNODIG
extern int Btgetallen[6];//BEETJE ONNODIG MISSCHIEN NIET HELEMAAL

void init(void);
void Stuur_LCD_Opdracht(unsigned char LCD_opdracht);
void Stuur_LCD_String(char *stringcharacters);
void LCD_Naar_Locatie(uint8_t x, uint8_t y);
void LCD_Scherm_Leeg(void);

#endif // LCDENBLUETOOTH_H_INCLUDED
