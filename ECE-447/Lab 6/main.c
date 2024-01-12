// keypad1.c - return value from telephone keypad
// Minimal test harness in main()
// MSPF2001 with keypad connected to port 1; needs internal pullups
// J H Davies, 2007-04-28; IAR Kickstart version 3.42A
//
// P1.7 "ignored"
// Keypad arranged like this on telephone keypad with * = 10 and # = 11:
//         P1.6 P1.5 P1.4
//          X1   X2   X3
// P1.0 Y1:  1    2    3
// P1.1 Y2:  4    5    6
// P1.2 Y3:  7    8    9
// P1.3 Y4:  *    0    #
// Outer loop of scan goes in columns from right to left
// and in rows from top to bottom within each column.
// Index values returned by scan procedure are:
//         P1.6 P1.5 P1.4
//          X1   X2   X3
// P1.0 Y1:  8    4    0
// P1.1 Y2:  9    5    1
// P1.2 Y3: 10    6    2
// P1.3 Y4: 11    7    3
//----------------------------------------------------------------------
#include <msp430.h>
#include <intrinsics.h>             // Intrinsic functions
#include <stdint.h>                 // Standard integer types

#define NOKEYPRESSED    -1
#define KEYERROR        -2

#define NCOLS           4
#define NROWS           4
#define ROWSMASK        0x0F
#define COLSMASK        0xF0

// Values associated with keys located by index
const int8_t keyValue[] = {10, 11, 12, 13, 3, 6, 9, 14, 2, 5, 8, 0, 1, 4, 7, 15};
unsigned char i = 0;
unsigned char count = 0;
uint16_t value = 0;
uint16_t first = 0;
uint16_t second = 0;
uint16_t third = 0;
// Function prototypes
void KeypadInit (void);
int lcd_init(void);
void lcd_clear(void);
void displayNum(unsigned int, unsigned int);
void showDigit(char, unsigned int);
int16_t KeypadRead (void);

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;       // Unlock ports from power manager
    P1DIR |= BIT0;
    P2DIR |= BIT6 | BIT7;       //P2.6 and P2.7 set as output
    P3DIR |= BIT6 | BIT7;       //P3.6 and P3.7 set as output
    P2IES |= (BIT0 | BIT1 | BIT2 |BIT3);
    P2IFG &= ~(BIT0 | BIT1 | BIT2 |BIT3);
    P2IE |= (BIT0 | BIT1 | BIT2 |BIT3);
    KeypadInit();
    lcd_init();
    TA2CTL = TASSEL_1 | ID_0 | MC_2 | TACLR;
    TB0CTL = TBSSEL__ACLK | ID__4 | MC__UP | TBCLR;
    TB0CCTL2 = TB0CCTL3 = TB0CCTL5 = TB0CCTL6 = OUTMOD_7;
    TB0CCR0 = 100;
    P2SEL1 &= ~(BIT6 | BIT7);   //connect P2.6 and P2.7
    P2SEL0 |= BIT6 | BIT7;      //to TB0.5 and TB0.6
    P3SEL1 |= BIT6 | BIT7;      //connect P3.6 and P3.7
    P3SEL0 &= ~(BIT6 | BIT7);   //to TB0.2 and TB0.3

    TB0CCR2 = 0; //P3.6
    TB0CCR3 = 0; //P3.7
    TB0CCR5 = 0; //P2.6
    TB0CCR6 = 0; //P2.7

    __enable_interrupt();

    while(1) {
        // Could wait for interrupt from P1IFG here
        __bis_SR_register(LPM3_bits);
    }
}

void KeypadInit (void) {
    //P2DIR = COLSMASK;               // Rows input, columns output
    P2DIR &= ~(BIT0 | BIT1 | BIT2 | BIT3);
    P8DIR |= (BIT4 | BIT5 | BIT6 | BIT7);
    //P2OUT = ROWSMASK;
    P2OUT |= (BIT0 | BIT1 | BIT2 | BIT3);
    P8OUT &= ~(BIT4 | BIT5 | BIT6 | BIT7);
    //P2REN = ROWSMASK;               //  Internal pullups on rows
    P2REN |= (BIT0 | BIT1 | BIT2 | BIT3);
    P8REN &= ~(BIT4 | BIT5 | BIT6 | BIT7);
}

int16_t KeypadRead (void) {
    uint8_t misses;             // Counts columns with no presses
    uint8_t index;              // Index of key pressed
    uint8_t colMask;            // Selects column being processed
    uint8_t column;             // Column being processed
    int16_t value;              // Value to be returned

    if ((P2IN & ROWSMASK) == ROWSMASK) {
        value = NOKEYPRESSED;   // No key pressed: don't scan
    } else {
        misses = 0;
        colMask = 0x10;         // Start with rightmost column
        for (column = 0; column < NCOLS*NROWS; column += NROWS) {
            P8DIR = colMask;    // All input except selected column,
            P8OUT = ~colMask;   //   pulled high
            P8REN = ~colMask;   // Pullups on all except selected column
            for(i=2500;i>0;i--);
            switch ((~P2IN) & ROWSMASK) {   // Test valid possibilities
                case 0x01:  index = column;     break;
                case 0x02:  index = column + 1; break;
                case 0x04:  index = column + 2; break;
                case 0x08:  index = column + 3; break;
                default:    ++misses;           break;
            }
            colMask <<= 1;      // Move to next column left
        }
        if (misses == (NCOLS-1)) {  // Only one key pressed, correct
            value = keyValue[index];
        } else {                    // Invalid input
            value = KEYERROR;
        }
        KeypadInit();           // Restore default settings
    }
    return value;
}

#pragma vector = PORT2_VECTOR
__interrupt void Port_2(void){

    value = KeypadRead();

    if (value > 9) {
        switch (value) {
            case 10:
                if (count == 0)
                    TB0CCR3 = 0; //Blue
                else if (count == 1)
                    TB0CCR3 = first; //Blue
                else if (count == 2)
                    TB0CCR3 = second; //Blue
                else if (third == 100)
                    TB0CCR3 = third; //Blue
                P1OUT |= BIT0;
                for(i=50000;i>0;i--);
                P1OUT &= ~BIT0;
                break;
            case 11:
                if (count == 0)
                    TB0CCR2 = 0; //Red
                else if (count == 1)
                    TB0CCR2 = first; //Red
                else if (count == 2)
                    TB0CCR2 = second; //Red
                else if (third == 100)
                    TB0CCR2 = third; //Red
                P1OUT |= BIT0;
                for(i=50000;i>0;i--);
                P1OUT &= ~BIT0;
                break;
            case 12:
                if (count == 0)
                    TB0CCR5 = 0; //Yellow
                else if (count == 1)
                    TB0CCR5 = first; //Yellow
                else if (count == 2)
                    TB0CCR5 = second; //Yellow
                else if (third == 100)
                    TB0CCR5 = third; //Yellow
                P1OUT |= BIT0;
                for(i=50000;i>0;i--);
                P1OUT &= ~BIT0;
                break;
            case 13:
                if (count == 0)
                    TB0CCR6 = 0; //Green
                else if (count == 1)
                    TB0CCR6 = first; //Green
                else if (count == 2)
                    TB0CCR6 = second; //Green

                else if (third == 100)
                    TB0CCR6 = third; //Green
                P1OUT |= BIT0;
                for(i=50000;i>0;i--);
                P1OUT &= ~BIT0;
                break;
            case 14:
                lcd_clear();
                displayNum(0, 1);
                first = 0;
                second = 0;
                third = 0;
                count = 0;
                break;
            case 15:
                if (count == 0) {
                    lcd_clear();
                    displayNum(0, 1);
                    first = 0;
                }
                else if (count == 1) {
                    lcd_clear();
                    displayNum(first, 1);
                    second = 0;
                    count = 0;
                }
                else if (count == 2) {
                    lcd_clear();
                    displayNum(second, 2);
                    third = 0;
                    count = 1;
                }
                else if (count == 3) {
                    lcd_clear();
                    displayNum(second, 2);
                    third = 0;
                    count = 2;
                }
                break;
        }
        P2IFG &= ~(BIT0 | BIT1 | BIT2 |BIT3);
        TA2CCR0  = TA2R+2500;  // set debounce wait time
        return;
    }

    if (count == 0 && value != 0) {
        first = value;
        lcd_clear();
        displayNum(first, 1);
        count++;
    }
    else if (count == 1) {
        second = value + first*10;
        lcd_clear();
        displayNum(second, 2);
        count++;
    }
    else if (count == 2 && first == 1 && second == 10 && value == 0) {
        third = second*10;
        displayNum(third, 3);
        count++;
    }



    P2IFG &= ~(BIT0 | BIT1 | BIT2 |BIT3);
    TA2CCR0  = TA2R+2500;  // set debounce wait time
}

#pragma vector= TIMER2_A0_VECTOR // Debunce delay is up
__interrupt void TIMER2_A0_ISR(void) {
    P2IE |= BIT0 | BIT1 | BIT2 | BIT3;           // re-enable button interrupt
    if (P2IES & (BIT0 | BIT1 | BIT2 | BIT3)) {               // currently on falling edge?
        P2IES &= ~(BIT0 | BIT1 | BIT2 | BIT3);   // set to rising edge

    }
    else
        P2IES |= BIT0 | BIT1 | BIT2 | BIT3;      // set to falling edge
    TA2CCTL0 &= ~CCIE;                   // disable this timer interrupt
    P2IFG &= ~(BIT0 | BIT1 | BIT2 | BIT3);       // changing P1IES can cause interrupts!
}
