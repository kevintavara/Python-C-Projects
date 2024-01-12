// LEDs should be connected to P2.6, P2.7, P3.6, P3.7
//
#include <msp430.h>

void msp_init()
{
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    P2DIR   |= BIT6 | BIT7;      // P2.6 and P2.7 set as output
    P2SEL1  &= ~(BIT6 | BIT7);   // connect P2.6 and P2.7
    P2SEL0  |= BIT6 | BIT7;      // to TB0.5 and TB0.6
    P3DIR   |= BIT6 | BIT7;      // P3.6 and P3.7 set as output
    P3SEL1  |= BIT6 | BIT7;      // connect P3.6 and P3.7
    P3SEL0  &= ~(BIT6 | BIT7);   // to TB0.2 and TB0.3

	TB0CCR0 = 100;				// (32768 Hz / 4) / 100 ticks = 81.92 Hz
	TB0CCR2 = 20;				// 20 / 100 = 20% duty cycle
	TB0CCR3 = 40;				// 40 / 100 = 40% duty cycle
	TB0CCR5 = 60;				// 60 / 100 = 60% duty cycle
	TB0CCR6 = 80;				// 80 / 100 = 80% duty cycle

	// set output mode to reset/set (see page 459 in user's guide - slau367f)
	TB0CCTL2 = TB0CCTL3 = TB0CCTL5 = TB0CCTL6 = OUTMOD_7;
	// clock source = ACLK divided by 4, put timer in UP mode, clear timer register
	TB0CTL = TBSSEL__ACLK | ID__4 | MC__UP | TBCLR;

	PM5CTL0 &= ~LOCKLPM5;		// Unlock ports from power manager
}

void main(void)
{
    msp_init();

	while(1)							// Enter low power mode
		__bis_SR_register(LPM3_bits);	// ACLK stays on in LPM3
}
