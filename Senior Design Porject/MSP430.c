/*--------------------------------------------------------
GEORGE MASON UNIVERSITY
ECE 447 - eUSCI_B0 Master talking to eUSCI_A0 slave on same LaunchPad
  Master sends counter data every 0.5 seconds and slave echoes it back
  Requires the following connections
  MASTER            SLAVE
  UCB0SOMI P1.7 --- UCA0SOMI P4.3
  UCB0SIMO P1.6 --- UCA0SIMO P4.2
  UCB0CLK  P1.4 --- UCA0CLK  P1.5

  LED 1 P1.0 on when data is sent to slave
  LED 2 P9.7 on when received data is same as sent

  Timer A0 used to trigger sending every 0.5 seconds

Date:   Fall 2014
Author: Jens-Peter Kaps

Change Log:
20160327 Updated for FR6989
--------------------------------------------------------*/
#include <msp430.h> 
#include "SPI-UCB0-UCA0.h"

unsigned char MST_Data,SLV_Data;
/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	              // Stop watchdog timer

    /* Buttons and LEDs */
    P1DIR    |= LED_RED;         // output, red LED
    P9DIR    |= LED_GREEN;       // output, green LED
    P1DIR    &= ~BUTTON1;        // button set as input
    P1OUT    |= BUTTON1;         // give pullup/down resistor a '1' so it pulls up
    P1REN    |= BUTTON1;         // enable pullup/down resistor

    MST_Data  = 0x01;            // Initialize data values
    SLV_Data  = 0x00;            //

    // MASTER SPI
    P1SEL1   &= ~(M_MOSI | M_MISO | M_SCLK);  // Connect UCB0
    P1SEL0   |=  M_MOSI | M_MISO | M_SCLK;
    UCB0CTLW0 |= UCSWRST;                      // Reset UCB0
    UCB0CTLW0 |= UCCKPL | UCMSB | UCMST | UCMODE_0 | UCSYNC;
    // SPI-mode 3 CKPL=1, /CKPH=0, MSB first, Master, 3-pin SPI, Synchronous
    // This mode allows us to not use Slave Select (SS), else its needed for timing
    UCB0CTLW0 |= UCSSEL_2;                     // SMCLK
    UCB0BR0 = 0x60;                           // Clock = SMCLK/60
    UCB0BR1 = 0;                              //
    UCB0CTL1 &= ~UCSWRST;                     // Release from Reset
    UCB0IE |= UCRXIE;                         // Enable USCI_B0 receive interrupt

    // SLAVE SPI
    P1SEL1   |= S_SCLK;                       // Connect to UCA0
    P1SEL0   &= ~S_SCLK;
    P4SEL1   &= ~(S_MOSI | S_MISO);
    P4SEL0   |= S_MOSI | S_MISO;
    UCA0CTLW0 |= UCSWRST;                      // Reset UCA0
    UCA0CTLW0 |= UCCKPL | UCMSB | UCMODE_0 | UCSYNC;
    // SPI-mode 3 CKPL=1, /CKPH=0, MSB first, 3-pin SPI, Synchronous
    UCA0CTLW0 &= ~UCSWRST;                     // Release from Reset
    UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt

    // Timer A0 for 0.5 sec interrupts
    TA0CTL = MC_1 | ID_0 | TASSEL_1 | TACLR;  // Up mode
                                              // Setup and start timer A in Up mode
                                              // ACLK/1, f=32768Hz = 2^15,
                                              // 2^16 (counter)/2^15=2 seconds till full
    TA0CCR0  = 0x4000;                        // 0.5sec=2sec/4=2^16/4=0x10000/4=0x4000
    TA0CCTL0 = CCIE;                          // Interrupt to send new value

    PM5CTL0 &= ~LOCKLPM5;       // Unlock ports from power manager

    __enable_interrupt();

    while(1){
    	__low_power_mode_3();   // going to sleep but keep ACLK running
    }
	return 0;
}

// SPI MASTER ISRs

//Interrupt Service Routine for Timer A0 channel 0
#pragma vector = TIMER0_A0_VECTOR   // associate funct. w/ interrupt vector
__interrupt void T0A0_ISR(void)     // name of ISR (can be anything)
{
	P9OUT &= ~LED_GREEN;    // green off
//    P9OUT ^= LED_GREEN;
	P1OUT |= LED_RED;
	while (!(UCB0IFG&UCTXIFG));               // USCI_B0 TX buffer ready?
//    while(UCB0STAT&UCBUSY);
	UCB0TXBUF = MST_Data;                     // Transmit counter value
    MST_Data++;
    // SMCLK remains active automatically for USCI
}

#pragma vector=USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
{
	switch(__even_in_range(UCB0IV,4)) {
	    case 0: break;                        // Vector 0 - no interrupt
        case 2:                               // Vector 2 - RXIFG
            P1OUT &= ~BIT0;
            if (UCB0RXBUF==SLV_Data)          // Test for correct character RX'd
                P9OUT |= LED_GREEN;           // If correct, light LED
            else
                P9OUT &= ~LED_GREEN;          // If incorrect, clear LED
            SLV_Data++;
            break;
        case 4: break;                        // Vector 4 - TXIFG
        default: break;
    }
}

// SPI SLAVE ISR
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
	switch(__even_in_range(UCA0IV,4))
    {
        case 0:break;                         // Vector 0 - no interrupt
        case 2:                               // Vector 2 - RXIFG
            while (!(UCA0IFG&UCTXIFG));       // USCI_A0 TX buffer ready?
            UCA0TXBUF = UCA0RXBUF;
            break;
        case 4:break;                         // Vector 4 - TXIFG
        default: break;
    }
}