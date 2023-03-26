/*
 * h_bridge.h - XvR 2020
 */

#ifndef _H_BRIDGE_H_
#define _H_BRIDGE_H_

// These pins are available on the shield via the header:
//
// digital 6	PH3
// digital 7	PH4
// digital 8	PH5
// digital 9	PH6

//

// The settings below are for the Mega, modify
// in case you want to use other pins
//H_brug 1
#define PORT_RPWM	PORTE
#define PIN_RPWM	PE4     //( OC1A/PCINT5 )       digital pin 2
#define DDR_RPWM	DDRE

#define PORT_LPWM	PORTE
#define PIN_LPWM	PE5     //( OC2A/PCINT4 )       digital pin 3
#define DDR_LPWM	DDRE
////H_brug 2
//#define PORT_RPWM1	PORTG
//#define PIN_RPWM1	PG5     //  digital pin 46
//#define DDR_RPWM1	DDRG
//
//#define PORT_LPWM1	PORTE
//#define PIN_LPWM1	PE3     //  digital pin 45
//#define DDR_LPWM1	DDRE
//H_brug 3
#define PORT_RPWM2	PORTG
#define PIN_RPWM2   PG5     //( OC4A )              digital pin 4
#define DDR_RPWM2	DDRG

#define PORT_LPWM2	PORTE
#define PIN_LPWM2	PE3     //( OC2B )              digital pin 5
#define DDR_LPWM2	DDRE

void init_h_bridge(void);
void h_bridge_set_percentage(signed char percentage);
//void h_bridge_set_percentage1(signed char percentage);
void h_bridge_set_percentage2(signed char percentage);

#endif /* _H_BRIDGE_H_ */
