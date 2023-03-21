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
#define PORT_RPWM	PORTB
#define PIN_RPWM	PB5     //( OC1A/PCINT5 )       digital pin 11
#define DDR_RPWM	DDRB

#define PORT_LPWM	PORTB
#define PIN_LPWM	PB4     //( OC2A/PCINT4 )       digital pin 10
#define DDR_LPWM	DDRB
//H_brug 2
#define PORT_RPWM1	PORTL
#define PIN_RPWM1	PL3     //  digital pin 46
#define DDR_RPWM1	DDRL

#define PORT_LPWM1	PORTL
#define PIN_LPWM1	PL4     //  digital pin 45
#define DDR_LPWM1	DDRL
//H_brug 3
#define PORT_RPWM2	PORTH
#define PIN_RPWM2   PH6     //( OC4A )              digital pin 9
#define DDR_RPWM2	DDRH

#define PORT_LPWM2	PORTF
#define PIN_LPWM2	PF5     //( OC2B )               digital pin a5
#define DDR_LPWM2	DDRF

void init_h_bridge(void);
void h_bridge_set_percentage(signed char percentage);
void h_bridge_set_percentage1(signed char percentage);
void h_bridge_set_percentage2(signed char percentage);

#endif /* _H_BRIDGE_H_ */
