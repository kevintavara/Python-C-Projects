#include <lcd.h>

// LCD memory map for numeric digits
const char digit[10][2] =
{
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

void displayNum(unsigned int num, unsigned int digits)
{
    short i;
    for(i=digits; i!=0; i--){
        showDigit(num%10,digits-i);
        num=num/10;
    }
}

void showDigit(char c, unsigned int position)
{
    unsigned int mydigit;

    LCDMEM[digitpos[position]] = digit[c][0];
    mydigit = digit[c][1];
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
    //lcd_clear();
    displayNum(0,1);
    return 0;
}

void lcd_clear() {
    LCDCMEMCTL |= LCDCLRM;
}
