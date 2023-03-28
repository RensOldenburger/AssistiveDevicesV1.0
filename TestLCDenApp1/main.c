/*
LCD_Naar_Locatie(x, y); verandert waar je iets neerzet op het scherm(waar de cursor staat)
Stuur_LCD_String("x"); hiermee kan je teksten neerzetten op de plek van de cursor
LCD_Scherm_Leeg(); hiermee maak je het scherm leeg
sprintf(array, typevariabele bijv. "%d", variabele); hiermee kan je variabelen omzetten naar strings -- moet je een array voor maken
*/
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "LCDenBluetooth.h"

int main(void)
{
    init();

    while(1)
    {
        if (verandergetal != veranderandergetal)
        {
            LCD_Scherm_Leeg();
            LCD_Naar_Locatie(1, 1);
            switch(Bluetooth_Getal)
            {
            case 1:
                LCD_Naar_Locatie(4, 1);
                Stuur_LCD_String("Bluetooth");
                LCD_Naar_Locatie(4, 2);
                Stuur_LCD_String("Connected");
                break;
            case 2:
                LCD_Naar_Locatie(5, 1);
                Stuur_LCD_String("Bluetooth");
                LCD_Naar_Locatie(3, 2);
                Stuur_LCD_String("Disconnected");
                break;
            case 3:
                LCD_Naar_Locatie(5, 1);
                Stuur_LCD_String("Opbouwen");
                break;
            case 4:
                LCD_Naar_Locatie(5, 1);
                Stuur_LCD_String("Afbouwen");
                break;
            case 5:
                LCD_Naar_Locatie(4, 1);
                Stuur_LCD_String("Neerzetten");
                break;
            case 6:
                LCD_Naar_Locatie(1, 1);
                Stuur_LCD_String("!!!!Noodknop!!!!");
                break;
            }
            if (veranderandergetal != 0)
            {
                veranderandergetal = 0;
            }
            else
            {
                veranderandergetal++;
            }
        }
    }

    return 0;
}
