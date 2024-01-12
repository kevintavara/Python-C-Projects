#include <msp430.h>
#include <stdbool.h>
#include "eventcounter.h"
#include "lcd.h"

unsigned int count = 0;
unsigned int sum = 0;
float total = 0;
bool flag = true;

int main(void) {
    volatile unsigned int i;

    WDTCTL  = WDTPW | WDTHOLD;   // Stop watchdog timer
    P1DIR   |= REDPIN;           // output: red LED
    P1DIR   &= ~BUTTON1;        // input
    P1OUT   |= BUTTON1;          // give pullup/down resistor a '1' so it pulls up
    P1REN   |= BUTTON1;          // enable pullup/down

    P2DIR   |= SENSOR;         // output
    P2SEL0 &= ~BIT0;
    P2SEL1 |= BIT0;
    TB0CTL = TBSSEL_1 | ID_3 | MC_1 | TBCLR | TBIE;
    TB0CCR0 = 409;
    TB0CCTL6 = OUTMOD_3 | CCIE;
    TB0CCR6 = 100;

    PM5CTL0 &= ~LOCKLPM5;        // Unlock ports from power manager, causes interrupts!
    P1IES   |= BUTTON1;          // Interrupt on falling edge
    P1IFG   &= ~BUTTON1;         // clear interrupt flag
    P1IE    |= BUTTON1;          // enable interrupt

    ADC12CTL0 = ADC12SHT0_2 | ADC12ON;      // Sampling time, S&H=16, ADC12 on
    ADC12CTL1 = ADC12SHP;                   // Use sampling timer
    ADC12CTL2 |= ADC12RES_2;                // 12-bit conversion results
    ADC12MCTL0 |= ADC12INCH_9;              // A9 ADC input select; Vref=AVCC (MEM0)
    P9SEL1 |= BIT1;
    P9SEL0 |= BIT1;

    lcd_init();                  // initialize the LCD

    __enable_interrupt();

    count = 0;
    while(1)                      // continuous loop
    {
        ADC12CTL0 |= ADC12ENC | ADC12SC;    // Start sampling/conversion
        if (count == 5) {
            displayNum(total*100, 2);
            count = 0;
        }
        __low_power_mode_3();     // going to sleep but keep ACLK running
    }

    return 0;
}

// Interrupt Service Routine for Timer B0
#pragma vector=TIMERB1_VECTOR
__interrupt void TIMERB1_ISR(void)
{
  switch(__even_in_range(TB0IV,14))
  {
    case  0: break;                          // No interrupt
    case  2: break;                          // CCR1 not used
    case  4: break;                          // CCR2
    case  6: break;                          // CCR3
    case  8: break;                          // CCR4
    case 10: break;                          // CCR5
    case 12: if(flag) {                      // CCR6
                 TB0CCR6 = total*41;
             }
             else {
                 TB0CCR6 = 0;
             }
             break;
    case 14: P1OUT ^= BIT0;                  // TAR overflow
             sum += ADC12MEM0;
             count++;
             if (count == 5) {
                 total = (float) ((sum / 5.00) / 1250.00);
                 sum = 0;
             }
             break;                          // XOR output
    default: break;
  }
  __low_power_mode_off_on_exit();
}

// Interrupt Service Routine for Port 1
#pragma vector=PORT1_VECTOR       // associate funct. w/ interrupt vector
__interrupt void Port_1(void)     // name of ISR
{
    if(P1IFG & BUTTON1){             // button 1 caused interrupt
        flag = !flag;
        P1IFG &= ~BUTTON1;           // clear interrupt flag of P1 BUTTON1
        __low_power_mode_off_on_exit();
    }
}

const char digit[10][2] = {
    {0xFC, 0x28},  /* "0" LCD segments a+b+c+d+e+f+k+q */
    {0x60, 0x20},  /* "1" */
    {0xDB, 0x00},  /* "2" */
    {0xF3, 0x00},  /* "3" */
    {0x67, 0x00},  /* "4" */
    {0xB7, 0x00},  /* "5" */
    {0xBF, 0x00},  /* "6" */
    {0xE4, 0x00},  /* "7" */
    {0xFF, 0x00},  /* "8" */
    {0xF7, 0x00}   /* "9" */
};
const char digitpos[6] = {pos6, pos5, pos4, pos3, pos2, pos1};
void displayNum(unsigned long num, unsigned int decpoint) {
    short i, digits;
    if(num > 99999)
        digits=6;
    else
        if(num > 9999)
            digits=5;
        else
            if(num > 999)
                digits = 4;
            else
                if(num > 99)
                    digits = 3;
                else
                    if(num > 9)
                        digits = 2;
                    else
                        digits = 1;
    if(decpoint > 5)
        decpoint=0;
    if(decpoint >= digits)
        digits = decpoint+1;
    for(i=6; i >= digits; i--){
        LCDMEM[digitpos[i]] = 0;   // clearing unused digits
        LCDMEM[digitpos[i]+1] &= 0x02;
    }
    for(i=digits; i!=0; i--){
        showDigit(num%10,digits-i,decpoint);
        num=num/10;
    }
}
void showDigit(char c, unsigned int position, unsigned int decpoint) {
    unsigned int mydigit;
    LCDMEM[digitpos[position]] = digit[c][0];
    mydigit = digit[c][1];
    if(position!=0){
        if(position==decpoint)                            // Decimal point position
            mydigit |= BIT0;
        else
            mydigit &= ~BIT0;
    }
    LCDMEM[digitpos[position]+1] = mydigit;
}
int lcd_init() {
    PJSEL0 = BIT4 | BIT5;                   // For LFXT
    LCDCPCTL0 = 0xFFF0;     // Init. LCD segments 4, 6-15
    LCDCPCTL1 = 0xF83F;     // Init. LCD segments 16-21, 27-31
    LCDCPCTL2 = 0x00F8;     // Init. LCD segments 35-39
    PM5CTL0 &= ~LOCKLPM5;
    CSCTL0_H = CSKEY >> 8;                  // Unlock CS registers
    CSCTL4 &= ~LFXTOFF;                     // Enable LFXT
    do {
      CSCTL5 &= ~LFXTOFFG;                  // Clear LFXT fault flag
      SFRIFG1 &= ~OFIFG;
    }while (SFRIFG1 & OFIFG);               // Test oscillator fault flag
    CSCTL0_H = 0;                           // Lock CS registers
    LCDCCTL0 = LCDDIV__1 | LCDPRE__16 | LCD4MUX | LCDLP | LCDSON;
    LCDCVCTL = VLCD_1 | VLCDREF_0 | LCDCPEN;
    LCDCCPCTL = LCDCPCLKSYNC;               // Clock synchronization enabled
    LCDCMEMCTL = LCDCLRM;                   // Clear LCD memory
    LCDCCTL0 |= LCDON;
    return 0;
}
void lcd_clear() {
    LCDCMEMCTL |= LCDCLRM;
}
unsigned int Dec2BCD(unsigned int Value){
    unsigned int i;
    unsigned int Output;
    for (i = 16, Output = 0; i; i--) {
        Output = __bcd_add_short(Output, Output);
        if (Value & 0x8000)
            Output = __bcd_add_short(Output, 1);
        Value <<= 1;
    }
    return Output;
}
