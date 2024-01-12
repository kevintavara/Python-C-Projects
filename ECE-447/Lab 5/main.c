#include <msp430.h>
#include <stdbool.h>
#include "fonts.h"

bool start = false;
bool started = false;
bool who = false;
unsigned int i = 0;
unsigned int j = 0;
unsigned char rowcnt;               // row counter
unsigned char colcnt;               // column counter
unsigned char g_matrix[8];          // content for LED matrix
unsigned char next_matrix[8];          // content for LED matrix
unsigned char nextmatrix[8];          // content for LED matrix
unsigned char r_matrix[8];          // content for red LED matrix
unsigned char g_row;                // current row of the green LED matrix
unsigned char r_row;                // current row of the red LED matrix
#define PERIOD1 0x0008
#define PERIOD2 0x0004

// gets the pixel data from the 5x7 font for character 'c'
// and stores it in an array pointed to by '*matrix' address.
void getnextchar(unsigned char *matrix, char c)
{
    unsigned char col=0;
    unsigned char row=0;
    unsigned char bit=0x01;
    unsigned char pc= c - 0x20;
    while (row<8) {
        while (col <5) {
            if (font_5x7[pc][col] & bit)
                matrix[row] |= 0x01;
            matrix[row]=matrix[row] << 1;
            col++;
        }
        col = 0;
        bit = bit << 1;
        row++;
    }
}

void clear(unsigned char *matrix) {
    int index = 0;
    for(index=0; index<8; index++){
            matrix[index] = 0;
    }
}

void turn(unsigned char *matrix) {
    int index = 0;
    for(index=0; index<8; index++){
            matrix[index] = 0;
            if (index == 0)
                matrix[0] = 0x04;
    }
}

// shifts the array '*matrix' one bit to the left and inserts
// into the rightmost column the left column from the array '*nextchar'
void shiftchar(unsigned char *matrix, unsigned char *nextchar)
{
    unsigned row=0;
    while (row<8) {
        matrix[row] = matrix[row] << 1;
        matrix[row] |= (nextchar[row]&0x20) >> 5;
        nextchar[row] = nextchar[row] << 1;
        row++;
    }
}

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PM5CTL0 &= ~LOCKLPM5;       // Unlock ports from power manager

	P2DIR &= ~(BIT1 | BIT2 | BIT3 | BIT4);
    P2REN |= (BIT1 | BIT2 | BIT3 |BIT4);  // enable resistor
    P2OUT |= (BIT1 | BIT2 | BIT3 |BIT4);  // pull up resistor
	P2IES |= BIT1 | BIT2 | BIT3 | BIT4;     // P2 falling edge
	P2IFG &= ~(BIT1 | BIT2 | BIT3 | BIT4);  // P2 clear interrupt flag
	P2IE |= BIT1 | BIT2 | BIT3 | BIT4;      // P2 enable interrupt

	P2DIR |= BIT7;
    P3DIR |= (BIT6 | BIT7);
	// Row Initialization
    P2SEL0 |= BIT7;
    P2SEL1 &= ~BIT7;

    // Column Clock and Column done/Row Clock
    P3SEL0 &= ~(BIT6 | BIT7);
    P3SEL1 |= (BIT6 | BIT7);
    // LED Matrix column serial outputs P9.0 and P9.1
    P9DIR |= (BIT0 | BIT1);
    P9OUT &= ~(BIT0 | BIT1);

    TB0CCTL0 = CCIE;
    TB0CCR0 = PERIOD1;
    TB0CCTL2 = OUTMOD_3;
    TB0CCR2 = PERIOD2;
    TB0CTL = MC_2 | ID_0 | TBCLR | TASSEL_1;

    for(rowcnt=0; rowcnt<8; rowcnt++){
        g_matrix[rowcnt] = 0;
    }
    for(rowcnt=0; rowcnt<8; rowcnt++){
        r_matrix[rowcnt] = 0;
    }

    rowcnt = 0;                 // starting row
    colcnt = 0;                 // starting column

	__enable_interrupt();


	getnextchar(r_matrix, 'h');
	getnextchar(g_matrix, 'h');
	while(1) {
	    __low_power_mode_3();
	    getnextchar(r_matrix, 'i');
	    getnextchar(g_matrix, 'i');
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
    case P2IV_P2IFG1:    // Vector  4: P2.1 L
        if (!start) {
            start = true;
            if (who) {
                clear(g_matrix);
                turn(r_matrix);
            }
            else {
                clear(r_matrix);
                turn(g_matrix);
            }
            for(i=5000;i>0;i--);
            break;
        }
        if (who & r_matrix[0] < 128)
            r_matrix[0] = 2*r_matrix[0];
        else if(!who & g_matrix[0] < 128)
            g_matrix[0] = 2*g_matrix[0];
        break;
    case P2IV_P2IFG2:    // Vector  6: P2.2 U
        if (!start) {
            start = true;
            if (who) {
                clear(g_matrix);
                turn(r_matrix);
            }
            else {
                clear(r_matrix);
                turn(g_matrix);
            }
            for(i=5000;i>0;i--);
            break;
        }
        if (!started) {
            start = false;
            clear(g_matrix);
            clear(r_matrix);
            getnextchar(r_matrix, 'h');
            getnextchar(g_matrix, 'h');
        }
        break;
    case P2IV_P2IFG3:    // Vector  8: P2.3 D
        if (!start) {
            start = true;
            if (who) {
                clear(g_matrix);
                turn(r_matrix);
            }
            else {
                clear(r_matrix);
                turn(g_matrix);
            }
            for(i=5000;i>0;i--);
            break;
        }
        started = true;
        who = !who;
        if (who) {
            turn(r_matrix);
            clear(g_matrix);
        }
        else {
            clear(r_matrix);
            turn(g_matrix);
        }
        break;
    case P2IV_P2IFG4:    // Vector 10: P2.4 R
        if (!start) {
            start = true;
            if (who) {
                clear(g_matrix);
                turn(r_matrix);
            }
            else {
                clear(r_matrix);
                turn(g_matrix);
            }
            for(i=5000;i>0;i--);
            break;
        }
        if (who & r_matrix[0] > 1)
            r_matrix[0] = r_matrix[0]/2;
        else if (!who & g_matrix[0] > 1)
            g_matrix[0] = g_matrix[0]/2;
        break;
    case P2IV_P2IFG5: break;   // Vector 12: P2.5
    case P2IV_P2IFG6: break;   // Vector 14: P2.6
    case P2IV_P2IFG7: break;   // Vector 16: P2.7
    default: break;
    }
    for(i=5000;i>0;i--);

}

// Interrupt Service Routine for Timer B channel CCR0,
// active on falling edge on column clock
#pragma vector = TIMER0_B0_VECTOR   // associate funct. w/ interrupt vector
__interrupt void T0B0_ISR(void)     // name of ISR (can be anything)
{

    /*
    if (!start) {
        switch(j) {
            case 0:
                getnextchar(r_matrix, 'h');
                getnextchar(g_matrix, 'h');
                getnextchar(next_matrix, 'e');
                break;
            case 8:
                getnextchar(next_matrix, 'l');
                break;
            case 16:
                getnextchar(next_matrix, 'l');
                break;
            case 24:
                getnextchar(next_matrix, 'o');
                break;
            case 32:
                j = 0;
                break;

        }
        i++;
        if (i == 64) {
            shiftchar(r_matrix, next_matrix);
            shiftchar(g_matrix, next_matrix);
            i = 0;
            j++;
        }
    } */
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
