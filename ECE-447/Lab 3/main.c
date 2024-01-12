/*--------------------------------------------------------
GEORGE MASON UNIVERSITY
ECE 447 - Lab3 In-Lab Exercise
  P2.1 button
  P2.2 button
  P2.3 button
  P2.4 button

  P2.6 Row Clock
  P2.7 Row Initialization

  P9.0 LED column 0
  ...
  P9.3 LED column 3

Date:   Fall 2020
Author: Jens-Peter Kaps

Change Log:
20200917 Initial Version, Jens-Peter Kaps
--------------------------------------------------------*/

// create global variable to "remember" which columns are on and which are off

#include <msp430.h> 
#include <stdbool.h>

unsigned char rowcnt = 4;
unsigned char row = 4;
unsigned char col = 4;

void delay(void) {
    volatile unsigned loops = 2500; // Start the delay counter at 25,000
    while (--loops > 0);             // Count down until the delay counter reaches 0
}

int main(void)
{


    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer

    P2DIR &= ~(BIT1 | BIT2 | BIT3 |BIT4); // buttons as inputs
    P2REN |= (BIT1 | BIT2 | BIT3 |BIT4);  // enable resistor
    P2OUT |= (BIT1 | BIT2 | BIT3 |BIT4);  // pull up resistor
    P2IES   |= (BIT1 | BIT2 | BIT3 | BIT4);  // P2 falling edge
    P2IFG   &= ~(BIT1 | BIT2 | BIT3 | BIT4); // P2 clear interrupt flag
    P2IE    |= (BIT1 | BIT2 | BIT3 | BIT4);  // P2 enable interrupt

    P2DIR |= (BIT6 | BIT7);   // Make Port 2.6 and 2.7 outputs
    P2OUT &= ~(BIT6 | BIT7);  // Set ports to 0

    P9DIR |= (BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);  // Outputs for LED Matrix
    P8DIR |= BIT7;
    P9OUT &= ~(BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0); // columns 5-8
    P8OUT |= BIT7;

    PM5CTL0 &= ~LOCKLPM5;     // Unlock ports from power manager

    __enable_interrupt();

    P2OUT |= BIT6;           //   only set row clock 1
    delay();
    P2OUT &= ~BIT6;          //   set row init to 0
    P2OUT |= BIT7;
    delay();
    P2OUT |= BIT6;  //   Set row clock and row init 1
    delay();
    P2OUT &= ~BIT6;          //   set row init to 0
    P2OUT &= ~BIT7;
    delay();
    P2OUT |= BIT6;  //   Set row clock and row init 1
    delay();
    P2OUT &= ~BIT6;          //   set row init to 0
    delay();
    P2OUT |= BIT6;  //   Set row clock and row init 1
    delay();
    P2OUT &= ~BIT6;          //   set row init to 0
    delay();
    P2OUT |= BIT6;  //   Set row clock and row init 1
    delay();
    P2OUT &= ~BIT6;          //   set row init to 0
    delay();
    P2OUT |= BIT6;  //   Set row clock and row init 1
    delay();
    P2OUT &= ~BIT6;          //   set row init to 0
    delay();
    P2OUT |= BIT6;  //   Set row clock and row init 1
    delay();
    P2OUT &= ~BIT6;          //   set row init to 0

    while(1)
    {
        if (col == 7) {
            P9OUT |= BIT0;  // Outputs for LED Matrix
            P8OUT &= ~BIT7;
            P9OUT &= ~(BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1); // columns 5-8
        }
        if (col == 6) {
            P9OUT |= BIT1;  // Outputs for LED Matrix
            P8OUT &= ~BIT7;
            P9OUT &= ~(BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT0); // columns 5-8
        }
        if (col == 5) {
            P9OUT |= BIT2;  // Outputs for LED Matrix
            P8OUT &= ~BIT7;
            P9OUT &= ~(BIT6 | BIT5 | BIT4 | BIT3 | BIT0 | BIT1); // columns 5-8
        }
        if (col == 4) {
            P9OUT |= BIT3;  // Outputs for LED Matrix
            P8OUT &= ~BIT7;
            P9OUT &= ~(BIT6 | BIT5 | BIT4 | BIT0 | BIT2 | BIT1); // columns 5-8
        }
        if (col == 3) {
            P9OUT |= BIT4;  // Outputs for LED Matrix
            P8OUT &= ~BIT7;
            P9OUT &= ~(BIT6 | BIT5 | BIT0 | BIT3 | BIT2 | BIT1); // columns 5-8
        }
        if (col == 2) {
            P9OUT |= BIT5;  // Outputs for LED Matrix
            P8OUT &= ~BIT7;
            P9OUT &= ~(BIT6 | BIT3 | BIT4 | BIT0 | BIT2 | BIT1); // columns 5-8
        }
        if (col == 1) {
            P9OUT |= BIT6;  // Outputs for LED Matrix
            P8OUT &= ~BIT7;
            P9OUT &= ~(BIT3 | BIT5 | BIT4 | BIT0 | BIT2 | BIT1); // columns 5-8
        }
        if (col == 0) {
            P8OUT |= BIT7;
            P9OUT &= ~(BIT6 | BIT5 | BIT4 | BIT3 | BIT0 | BIT2 | BIT1); // columns 5-8
        }
        if (row > rowcnt) {
            rowcnt = row;
            P2OUT |= BIT6;  //   Set row clock and row init 1
            delay();
            P2OUT &= ~BIT6;          //   set row init to 0
        }
        if (row < rowcnt) {
            unsigned char i = row;
            while(rowcnt < 7) {
                P2OUT |= BIT6;  //   Set row clock and row init 1
                delay();
                P2OUT &= ~BIT6;          //   set row init to 0
                rowcnt++;
            }
            P2OUT |= BIT7;
            delay();
            P2OUT |= BIT6;
            delay();
            P2OUT &= ~(BIT6 | BIT7);          //   set row init to 0
            delay();
            while(0 < i) {
                P2OUT |= BIT6;  //   Set row clock and row init 1
                delay();
                P2OUT &= ~BIT6;          //   set row init to 0
                i--;
            }

            rowcnt = row;
            i = 0;
        }

        delay();
    }
    return 0;
}

// P2 interrupt service routine
#pragma vector = PORT2_VECTOR      // associate funct. w/ interrupt vector
__interrupt void Port_2(void)      // name of ISR
{
    switch(__even_in_range(P2IV,P2IV_P2IFG7))
    {
    case P2IV_NONE:   break;   // Vector  0: no interrupt
    case P2IV_P2IFG0: break;   // Vector  2: P2.0
    case P2IV_P2IFG1: if(col > 0) {col--;} break;  // Vector  4: P2.1
    case P2IV_P2IFG2: if(row > 0) {row--;} break; // Vector  6: P2.2
    case P2IV_P2IFG3: if(row < 7) {row++;} break; // Vector  8: P2.3
    case P2IV_P2IFG4: if(col < 7) {col++;} break; // Vector 10: P2.4
    case P2IV_P2IFG5: break;   // Vector 12: P2.5
    case P2IV_P2IFG6: break;   // Vector 14: P2.6
    case P2IV_P2IFG7: break;   // Vector 16: P2.7
    default: break;
    }
}
