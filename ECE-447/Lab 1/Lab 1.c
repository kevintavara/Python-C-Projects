#include <msp430.h>
#include <stdbool.h>

/*
 * Swtich S1 - P1.1
 * LED1      - P1.0
 */

void delay(void) {
    volatile unsigned loops = 25000; // Start the delay counter at 25,000
    while (--loops > 0);             // Count down until the delay counter reaches 0
}

void main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
    bool flag = false;        // Flag to set blinking
    P2REN |= BIT1;            // Connect resistor on P1.1 to P1OUT
    P2OUT |= BIT1;
    P2DIR &= ~BIT1;
    P9DIR |= BIT5;            // Make P1.0 an output
    PM5CTL0 &= ~LOCKLPM5;     // Unlock ports from power manager

    for (;;) {
        //delay();              // Run the delay sub-routine
        if (!(P2IN & BIT1)) {  // Read the input from P1.1 and check its state
            flag = !flag;

            /* Press to turn on
            P2OUT |= BIT6; */
            P9OUT |= BIT5;
            /* Blink while pressed
            P2OUT |= BIT6;
            delay();
            P2OUT &= ~BIT6; */


        }

        /* Press to set blinking or turn off
        if (flag) {
            P2OUT |= BIT6;
            delay();
            P2OUT &= ~BIT6;
        }
        else {
            P2OUT &= ~BIT6;
        } */

        // Part of Press to turn on
        else {
            //P2OUT &= ~BIT6;   // If the button isn't pressed, turn off the LED
            P9OUT &= ~BIT5;
        } //
    }
}
