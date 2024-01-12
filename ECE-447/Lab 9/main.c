/*--------------------------------------------------------
GEORGE MASON UNIVERSITY
ECE 447 - Lab 9
  Requires TFT LCD to be connected as follows
  P1.3 LCD Backlight - TA1.2
  P1.4 SPI SCLK      - UCB0CLK
  P1.6 SPI MOSI      - UCB0SIMO
  P1.7 SPI MISO      - UCB0SOMI
  P2.5 SPI TFT CS
  P4.7 LCD D/C

  Test Data Output (4-bit counter at 1kHz)
  P9.1 LSB
  P9.2
  P9.3
  P9.4 MSB

  Logic Analyzer Data Inputs
  P3.0 LSB
  P3.1
  P3.2
  P3.3 MSB

Date:   Fall 2020
Author: Jens-Peter Kaps

Change Log:
20201101 Initial Version
--------------------------------------------------------*/

#include "lcd.h"
#include "graphics.h"
#include "color.h"
#include "ports.h"
#include "lab9.h"

int width = 20;
int i = 0;
int j = 0;
uint8_t first[255];
uint8_t second[255];
uint8_t third[255];
uint8_t fourth[255];
void introScreen(void) {
    clearScreen(1);
    setColor(COLOR_16_WHITE);
    drawString(2, 15, FONT_LG, "ECE 447 Lab 9");
    drawString(2, 35, FONT_SM, "4 Channel Logic Analyzer");
    setColor(COLOR_16_RED);
    drawString(2, 45, FONT_SM, "Due in one week!");
    setColor(COLOR_16_YELLOW);
    drawString(3, 70, FONT_MD, "Lab 9 Intro Screen!");
}

/*
 *  Needs to write data to the device using spi. We will only want to write to
 *  the device we wont worry the reads.
 */

void writeData(uint8_t data) {
    P2OUT &= ~BIT5;
    P4OUT |= BIT7;
    UCB0TXBUF = data;
    while(UCB0STAT&UCBUSY);
    P2OUT |= BIT5;
}

/*
 *  Needs to write commands to the device using spi
 */

void writeCommand(uint8_t command) {
    P2OUT &= ~BIT5;
    P4OUT &= ~BIT7;
    UCB0TXBUF = command;
    while(UCB0STAT&UCBUSY);
    P2OUT |= BIT5;
}

void initMSP430(void) {

    /*********************** Test Data Generator 1 kHz *********************/
    P9DIR   |= BIT1 | BIT2 | BIT3 | BIT4;   // Test Data Output
    TA2CTL   = TASSEL__SMCLK | MC__CONTINUOUS | TACLR;
    TA2CCR1  = 1000;                                    // 1MHz * 1/1000 Hz
    TA2CCTL1 = CCIE;                                    // enable interrupts
    testcnt  = 0;                                       // start test counter at 0

    TB0CTL   = TBSSEL_2 | MC_1 | TBCLR;
    TB0CCR0  = 100;                                    // 1MHz * 1/1000 Hz
    TB0CCTL0 = CCIE;                                    // enable interrupts
    P2DIR &= ~(BIT0 | BIT1 | BIT2 | BIT3);
    P2REN |= (BIT0 | BIT1 | BIT2 | BIT3);
    P2OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3);

    /**************************** PWM Backlight ****************************/

    P1DIR   |= BIT3;
    P1SEL0  |= BIT3;
    P1SEL1  &= ~BIT3;
    TA1CCR0  = 511;
    TA1CCTL2 = OUTMOD_7;
    TA1CCR2  = 255;
    TA1CTL   = TASSEL__ACLK | MC__UP | TACLR;

    /******************************** SPI **********************************/

    P2DIR  |=   LCD_CS_PIN;                     // DC and CS
    P4DIR  |=   LCD_DC_PIN;
    P1SEL0 |=   LCD_MOSI_PIN | LCD_UCBCLK_PIN;      // MOSI and UCBOCLK
    P1SEL1 &= ~(LCD_MOSI_PIN | LCD_UCBCLK_PIN);

    UCB0CTLW0 |= UCSWRST;       // Reset UCB0

    /*
     * UCBxCTLW0     - eUSCI_Bx Control Register 0
     * UCSSEL__SMCLK - SMCLK in master mode
     * UCCKPL        - Clock polarity select
     * UCMSB         - MSB first select
     * UCMST         - Master mode select
     * UCMODE_0      - eUSCI mode 3-pin SPI select
     * UCSYNC        -  Synchronous mode enable
     */
    //UCB0CTLW0 |= UCSSEL__SMCLK | UCCKPL | UCMSB | UCMST | UCMODE_0 | UCSYNC;
    UCB0CTLW0 |= UCSSEL__SMCLK | UCCKPH | UCMSB | UCMST | UCMODE_0 | UCSYNC;
    //    UCB0CTLW0 = UCCKPH + UCMSB + UCMST + UCSYNC + UCSSEL_2; // 3-pin, 8-bit SPI master


    UCB0BR0   |= 0x01;         // Clock = SMCLK/1
    UCB0BR1    = 0;
    UCB0CTL1  &= ~UCSWRST;     // Clear UCSWRST to release the eUSCI for operation
    PM5CTL0   &= ~LOCKLPM5;    // Unlock ports from power manager

    __enable_interrupt();

}

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // kill the watchdog

    P1REN = BIT1 | BIT2;        // Connect resistor on P1.1 to P1OUT
    P1OUT = BIT1 | BIT2;        // Set output register for P1.1 to '1'
    P1IES = BIT1 | BIT2;        // P1.1 Hi/Lo edge
    P1IFG = 0;                  // Clear all P1 interrupt flags
    P1IE = BIT1 | BIT2;         // P1.1 interrupt enabled
    initMSP430();

    __delay_cycles(10);

    initLCD();

    introScreen();

    clearScreen(1);

    int i;

    while (TRUE) {
        _nop();
        setColor(COLOR_16_WHITE);
        drawString(1, 0, FONT_MD, "CH Logic Analyzer");
        drawString(1, 15, FONT_MD, "1");
        drawString(1, 30, FONT_MD, "2");
        drawString(1, 45, FONT_MD, "3");
        drawString(1, 60, FONT_MD, "4");
        drawString(1, 75, FONT_MD, "Trigger: off");
        switch(j) {
            case 0:
                drawString(1, 100, FONT_MD, "Frequency: 10 kHz");
                TB0CCR0  = 100;
                break;
            case 1:
                drawString(1, 100, FONT_MD, "Frequency: 5 kHz");
                TB0CCR0  = 200;
                break;
            case 2:
                drawString(1, 100, FONT_MD, "Frequency: 2 kHz");
                TB0CCR0  = 500;
                break;
            case 3:
                drawString(1, 100, FONT_MD, "Frequency: 500 Hz");
                TB0CCR0  = 2000;
                break;
            case 4:
                drawString(1, 100, FONT_MD, "Frequency: 100 Hz");
                TB0CCR0  = 10000;
                break;
        }
        drawString(1, 110, FONT_MD, "Samples: 256");
        for(i = 0; i < 256; i++) {
            if(width < 160) {
                if (first[i] != first[i+1]) {
                    setColor(COLOR_16_YELLOW);
                    drawLine(width + 10, 30, width + 10, 20);
                }
                if (first[i] == 0) {
                    setColor(COLOR_16_YELLOW);
                    drawLine(width, 30, width + 10, 30);
                }
                else {
                    setColor(COLOR_16_YELLOW);
                    drawLine(width, 20, width + 10, 20);
                }
                if (second[i] != second[i+1]) {
                    setColor(COLOR_16_CYAN);
                    drawLine(width + 10, 45, width + 10, 35);
                }
                if (second[i] == 0) {
                    setColor(COLOR_16_CYAN);
                    drawLine(width, 45, width + 10, 45);
                }
                else {
                    setColor(COLOR_16_CYAN);
                    drawLine(width, 35, width + 10, 35);
                }
                if (third[i] != third[i+1]) {
                    setColor(COLOR_16_MAGENTA);
                    drawLine(width + 10, 60, width + 10, 50);
                }
                if (third[i] == 0) {
                    setColor(COLOR_16_MAGENTA);
                    drawLine(width, 60, width + 10, 60);
                }
                else {
                    setColor(COLOR_16_MAGENTA);
                    drawLine(width, 50, width + 10, 50);
                }
                if (fourth[i] != fourth[i+1]) {
                    setColor(COLOR_16_BLUE);
                    drawLine(width + 10, 75, width + 10, 65);
                }
                if (fourth[i] == 0) {
                    setColor(COLOR_16_BLUE);
                    drawLine(width, 75, width + 10, 75);
                }
                else {
                    setColor(COLOR_16_BLUE);
                    drawLine(width, 65, width + 10, 65);
                }
                width += 10;

            }
            else {
                clearScreen(1);
                setColor(COLOR_16_WHITE);
                drawString(1, 0, FONT_MD, "CH Logic Analyzer");
                drawString(1, 15, FONT_MD, "1");
                drawString(1, 30, FONT_MD, "2");
                drawString(1, 45, FONT_MD, "3");
                drawString(1, 60, FONT_MD, "4");
                drawString(1, 75, FONT_MD, "Trigger: off");

                switch(j) {
                    case 0:
                        drawString(1, 100, FONT_MD, "Frequency: 10 kHz");
                        TB0CCR0  = 100;
                        break;
                    case 1:
                        drawString(1, 100, FONT_MD, "Frequency: 5 kHz");
                        TB0CCR0  = 200;
                        break;
                    case 2:
                        drawString(1, 100, FONT_MD, "Frequency: 2 kHz");
                        TB0CCR0  = 500;
                        break;
                    case 3:
                        drawString(1, 100, FONT_MD, "Frequency: 500 Hz");
                        TB0CCR0  = 2000;
                        break;
                    case 4:
                        drawString(1, 100, FONT_MD, "Frequency: 100 Hz");
                        TB0CCR0  = 10000;
                        break;
                }
                drawString(1, 110, FONT_MD, "Samples: 256");
                width = 20;
            }
        }
        TB0CCTL0 = CCIE;
        __low_power_mode_1();

    }
}

// Interrupt Service Routine for Port 1
#pragma vector=PORT1_VECTOR       // associate funct. w/ interrupt vector
__interrupt void Port_1(void)     // name of ISR
{
    if(P1IFG & BIT1){             // button 1 caused interrupt
        P1IFG &= ~BIT1;           // clear interrupt flag of P1 BUTTON1
        if (j < 4)
            j++;
        __low_power_mode_off_on_exit();
    }
    if(P1IFG & BIT2){             // button 1 caused interrupt
        P1IFG &= ~BIT2;           // clear interrupt flag of P1 BUTTON1
        if (j > 0)
            j--;
        __low_power_mode_off_on_exit();
    }
}

// Interrupt Service Routine for Timer B0
// Only used for test data generation
#pragma vector=TIMER0_B0_VECTOR
__interrupt void TIMER0_B0_ISR(void)
{
  first[i] = (P2IN & BIT0) ? 0:1;
  second[i] = (P2IN & BIT1) ? 0:1;
  third[i] = (P2IN & BIT2) ? 0:1;
  fourth[i] = (P2IN & BIT3) ? 0:1;
  i++;
  if(i > 255) {
      TB0CCTL0 = ~CCIE;
      i = 0;
  }
  __low_power_mode_off_on_exit();
}

// Interrupt Service Routine for Timer A2
// Only used for test data generation
#pragma vector=TIMER2_A1_VECTOR
__interrupt void TIMER2_A1_ISR(void)
{
  switch(__even_in_range(TA2IV,2))
  {
    case  0: break;                             // No interrupt
    case  2:                                    // CCR1
        P9OUT   &= ((testcnt << 1) | 0xE1 );    // set 0 bits
        P9OUT   |= ((testcnt << 1) & 0x1E );    // set 1 bits
        testcnt ++;                             // increment counter
        testcnt &= 0x0F;                        // limit to 4 bits
        TA2CCR1 += 1000;                        // back here in 1000 timer cycles
        break;
    default: break;
  }
}
