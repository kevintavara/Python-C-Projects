/*--------------------------------------------------------
GEORGE MASON UNIVERSITY
ECE 447 - Lab2 Row Timing
  P2.6 Row Clock
  P2.7 Row Initialization

Date:   Fall 2020
Author: Jens-Peter Kaps

Change Log:
20200912 Initial Version, Jens-Peter Kaps
--------------------------------------------------------*/

#include <msp430.h>

void delay1(void) {
    volatile unsigned loops = 16000;
    while (--loops > 0);             // Count down until the delay counter reaches 0
}

void delay2(void) {
    volatile unsigned loops = 50000;
    while (--loops > 0);             // Count down until the delay counter reaches 0
}

int main(void)
{
    unsigned char rowcnt;     // row counter

    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer

    P2DIR |= (BIT6 | BIT7);   // Make Port 2.6 and 2.7 outputs
    P8DIR |= BIT7;
    P9DIR |= (BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);

    P2OUT &= ~(BIT6 | BIT7);  // Set ports to 0
    P8OUT &= ~BIT7;
    P9OUT &= ~(BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
    PM5CTL0 &= ~LOCKLPM5;     // Unlock ports from power manager

    rowcnt = 0;
    while(1)                    // continuous loop
    {
        P9OUT |= BIT0;
        delay1();
        if(P2OUT & BIT6)  {               // If row clock 1 -> place breakpoint here
            P2OUT &= ~BIT6;              //   Set row clock 0
        }
        else {
            if(rowcnt == 7) {            // if on row 7
                P2OUT |= (BIT6 | BIT7);  //   Set row clock and row init 1
                P8OUT |= BIT7;           //   only set row clock 1
                rowcnt = 0;              //   set row counter back to 0
            }
            else if(rowcnt == 6) {
                P2OUT |= BIT6;           //   only set row clock 1
                P2OUT &= ~BIT7;          //   set row init to 0
                P9OUT |= BIT6;
                rowcnt++;
            }
            else if(rowcnt == 5) {
                P2OUT |= BIT6;           //   only set row clock 1
                P2OUT &= ~BIT7;          //   set row init to 0
                P9OUT |= BIT5;
                rowcnt++;
            }
            else if(rowcnt == 4) {
                P2OUT |= BIT6;           //   only set row clock 1
                P2OUT &= ~BIT7;          //   set row init to 0
                P9OUT |= BIT4;
                rowcnt++;
            }
            else if(rowcnt == 3) {
                P2OUT |= BIT6;           //   only set row clock 1
                P2OUT &= ~BIT7;          //   set row init to 0
                P9OUT |= BIT3;
                rowcnt++;
            }
            else if(rowcnt == 2) {
                P2OUT |= BIT6;           //   only set row clock 1
                P2OUT &= ~BIT7;          //   set row init to 0
                P9OUT |= BIT2;
                rowcnt++;
            }
            else if(rowcnt == 1) {
                P2OUT |= BIT6;           //   only set row clock 1
                P2OUT &= ~BIT7;          //   set row init to 0
                P9OUT |= BIT1;
                rowcnt++;
            }
            else {
                P2OUT |= BIT6;           //   only set row clock 1
                P2OUT &= ~BIT7;          //   set row init to 0
                rowcnt++;
            }
        }

    }

    return 0;
}
