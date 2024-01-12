/*--------------------------------------------------------
GEORGE MASON UNIVERSITY
ECE 447 - Lab 4 - LED Matrix test for red and green LEDs skeleton
  Using timer ISR for multiplexing

  P2.6 unused
  P2.7 Row Initialization (CCR6)
  P3.6 Column Clock (CCR2)
  P3.7 Column Done  (CCR3) is now also Row Clock
  P9.0 Green Row Data (serial, one column at a time with LSB first,
                 LSB will show as column 8 in matrix)
  P9.1 Red Row Data (serial, one column at a time with LSB first,
                 LSB will show as column 8 in matrix)


Date:   Fall 2020
Author: Jens-Peter Kaps

Change Log:
20200928 Initial Version, Jens-Peter Kaps
--------------------------------------------------------*/

#include <msp430.h> 
#include <stdbool.h>

unsigned char rowcnt;               // row counter
unsigned char colcnt;               // column counter
unsigned char g_matrix[8];          // content for LED matrix
unsigned char r_matrix[8];          // content for red LED matrix
unsigned char g_row;                // current row of the green LED matrix
unsigned char r_row;                // current row of the red LED matrix
bool flag = true;
#define PERIOD1 0x0008
#define PERIOD2 0x0004
/*
 * main.c
 */
int main(void)
{
    unsigned int i;                       // all purpose
    WDTCTL = WDTPW | WDTHOLD;             // Stop watchdog timer
    // Add code to:
    // connect the ports P2.7, P3.6, and P3.7 to timer
    // B0 CCR6, CCR2, and CCR3 respectively
    P2DIR |= BIT7;
    P3DIR |= (BIT6 | BIT7);

    P2SEL0 |= BIT7;
    P2SEL1 &= ~BIT7;

    P3SEL0 &= ~(BIT6 | BIT7);
    P3SEL1 |= (BIT6 | BIT7);
    // Add code to:
    // make the LED Matrix column serial outputs P9.0 and P9.1 outputs
    // and output a 0 on each.
    P9DIR |= (BIT0 | BIT1);
    P9OUT &= ~(BIT0 | BIT1);

    PM5CTL0 &= ~LOCKLPM5;                 // Unlock ports from power manager
    // CCR2 is connected to P3.6 which is column clock.
    // CCR0 active with interrupt, column clock goes low (set/RESET).
    // When CCR2 triggers (no interrupt) column clock goes high (SET/reset).
    // CCR0 and CCR2 are 4 timer clock cycles apart.

    // Add code to:
    // Initialize TB0CCR0 and TB0CCR2 such that CCR0 has an event 4 clock cyles after CCR2
    // Enable interrupts for CCR0
    // Set the out mode for CCR2
    TB0CCTL0 = CCIE;
    TB0CCR0 = PERIOD1;

    TB0CCTL2 = OUTMOD_3;
    TB0CCR2 = PERIOD2;
    // Add code to:
    // Setup and start timer B in continuous mode, ACLK, CLK/1, clear timer
    TB0CTL = MC_2 | ID_0 | TBCLR | TASSEL_1;
    // Initialize matrix with test pattern
    i=2;
    for(rowcnt=0; rowcnt<8; rowcnt++){
        g_matrix[rowcnt]=i-1;
        i=i<<1;
    }
    i=255;
    for(rowcnt=0; rowcnt<8; rowcnt++){
        r_matrix[rowcnt]=i;
        i=i<<1;
    }
    rowcnt = 0;                 // starting row
    colcnt = 0;                 // starting column

    __enable_interrupt();

    while(1)                    // continuous loop
    {
        __low_power_mode_3();
    }
    return 0;
}

// Interrupt Service Routine for Timer B channel CCR0,
// active on falling edge on column clock

#pragma vector = TIMER0_B0_VECTOR   // associate funct. w/ interrupt vector
__interrupt void T0B0_ISR(void)     // name of ISR (can be anything)
{

    // Add code to:
    // output one bit (column) of the green and of the red row
    // and then shift them to move to the next column
    if(r_row & 0x01)              //   output one bit (column) of current row
        P9OUT |= BIT0;
    else
        P9OUT &= ~BIT0;
    r_row = r_row >>1;              //   move to next column in row

    if(g_row & 0x01)              //   output one bit (column) of current row
        P9OUT |= BIT1;
    else
        P9OUT &= ~BIT1;
    g_row = g_row >>1;              //   move to next column in row
    // Add code to:
    // create timer events for CCR0 and CCR2.
    // both 8 clock cycles from the last one
    TB0CCR0 += PERIOD1;
    TB0CCR2 += PERIOD1;
    if(colcnt == 7)                 // When on last column of matrix
    {
      // Add code to:
      // create events for column_done and row_init based upon the
      // specifications in the lab manual
      // At last column set CCR3 to falling edge
      TB0CCTL3 = OUTMOD_1;
      TB0CCR3 = TB0CCR0;
      // Add code to:
      // increment the row counter and set the column counter back to 0
      rowcnt++;
      // At last row set CCR6 to rising edge
      if(rowcnt == 8) {
          rowcnt = 0;
          TB0CCTL6 = OUTMOD_1;
          TB0CCR6 = TB0CCR2;
      }
      colcnt = 0;
      // Add code to:
      // update the current row for red and green
      g_row = g_matrix[rowcnt];
      r_row = r_matrix[rowcnt];
    } else {
      // Add code to:
      // create events for column_done and row_init based upon the
      // specifications in the lab manual
      TB0CCTL3 = OUTMOD_5;
      TB0CCTL6 = OUTMOD_5;
      TB0CCR3 = TB0CCR0;
      TB0CCR6 = TB0CCR2;
      // Add code to:
      // increment the column counter
      colcnt++;
    }

}
