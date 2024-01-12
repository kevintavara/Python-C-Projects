#include <msp430.h>
#include <stdio.h>
#include <stdbool.h>

volatile unsigned char RXData;
unsigned char i = 0;
unsigned char j = 0;

bool dirX = true;
bool dirY = true;
bool dirZ = true;

int X_current = 20;
int Y_current = 25;
int Z_current = 30;

signed int X_temp = 50;
signed int Y_temp = 75;
signed int Z_temp = 90;

signed int X_mag = 0;
signed int Y_mag = 0;
signed int Z_mag = 0;

signed int dX = 0;
signed int dY = 0;
signed int dZ = 0;

int main(void) {

   WDTCTL = WDTPW | WDTHOLD;                 // stop watchdog
  // Configure UART
  P2SEL0 |= BIT0 | BIT1;                    // Configure UART pins
  P2SEL1 &= ~(BIT0 | BIT1);
  PJSEL0 |= BIT4 | BIT5;                    // Configure XT1 pins
  // Configure H-Bridge
  P2DIR   |= BIT3 | BIT6 | BIT7;            // P2.6 and P2.7 set as output
  P2SEL1  &= ~(BIT6 | BIT7);            // connect P2.6 and P2.7 (x, y)
  P2SEL0  |= BIT6 | BIT7;               // to TB0.5 and TB0.6
  P3DIR   |= BIT0 | BIT1 | BIT6;            // P3.6 set as output
  P3SEL1  |= BIT6;                      // connect P3.6 (z)
  P3SEL0  &= ~BIT6;                     // to TB0.2
  P2OUT   &= ~(BIT3);               // DIR of z-axis
  P3OUT   &= ~(BIT0 | BIT1);            // DIR of x-axis and y-axis
  P3OUT   |= (BIT0 | BIT1);            // DIR of x-axis and y-axis
  TB0CCR0 = 100;                        // (32768 Hz / 4) / 100 ticks = 81.92 Hz
  // Anything above 32% is above 5.5A
  TB0CCR2 = 0;                         // 20 / 100 = 20% duty cycle (z-axis)
  TB0CCR5 = 0;                         // 60 / 100 = 60% duty cycle (x-axis)
  TB0CCR6 = 0;                         // 80 / 100 = 80% duty cycle (y-axis)
  TB0CCTL2 = TB0CCTL5 = TB0CCTL6 = OUTMOD_7;// set output mode to reset/set
  // clock source = ACLK divided by 4, put timer in UP mode, clear timer register
  TB0CTL = TBSSEL__SMCLK | ID_0 | MC__UP | TBCLR;

  // Disable the GPIO power-on default high-impedance mode to activate
  // previously configured port settings
  PM5CTL0 &= ~LOCKLPM5;

  // Configure ADC12
  P8SEL0 |= (BIT4 | BIT5 | BIT6);
  P8SEL1 |= (BIT4 | BIT5 | BIT6);
  P9SEL0 |= (BIT0 | BIT1 | BIT5); //ADC outputs for X, Y, and Z current sensors
  P9SEL1 |= (BIT0 | BIT1 | BIT5);
  /* Initialize TA2 */

  TA2CTL = MC_1 | ID_3 | TASSEL_1 | TACLR;
  TA2EX0 = TAIDEX_7;
  TA2CCR0 = 0x0034;
  TA2CCTL1 = OUTMOD_3;
  TA2CCR1 = 0x0033;

  // ADC Setup
  ADC12CTL0 &= ~(ADC12ENC);
  ADC12CTL0 = ADC12SHT0_2 | ADC12MSC | ADC12ON;      // Sampling time, S&H=16, ADC12 on
  ADC12CTL1 = ADC12SHP | ADC12CONSEQ_1 | ADC12SHS_0;   // Use sampling timer
  ADC12CTL2 |= ADC12RES_2;                // 12-bit conversion results
  ADC12MCTL0 |= ADC12INCH_7 | ADC12VRSEL_0; // A8 ADC input select; Vref=AVCC (MEM0)
  ADC12MCTL1 |= ADC12INCH_6 | ADC12VRSEL_0; // A8 ADC input select; Vref=AVCC (MEM0)
  ADC12MCTL2 |= ADC12INCH_5 | ADC12VRSEL_0; // A8 ADC input select; Vref=AVCC (MEM0)
  ADC12MCTL3 |= ADC12INCH_8 | ADC12VRSEL_0; // A8 ADC input select; Vref=AVCC (MEM0)
  ADC12MCTL4 |= ADC12INCH_9 | ADC12VRSEL_0; // A8 ADC input select; Vref=AVCC (MEM0)
  ADC12MCTL5 |= ADC12INCH_13 | ADC12EOS | ADC12INIE;    // A8 ADC input select; Vref=AVCC (MEM0)
  ADC12IER0 |= ADC12IE5;                    // Enable ADC conv complete interrupt


  // XT1 Setup
  CSCTL0_H = CSKEY >> 8;                    // Unlock CS registers
  CSCTL1 = DCOFSEL_0;                       // Set DCO to 1MHz
  CSCTL2 = SELA__LFXTCLK | SELS__DCOCLK | SELM__DCOCLK;
  CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Set all dividers
  CSCTL4 &= ~LFXTOFF;                       // Enable LFXT1
  do
  {
    CSCTL5 &= ~LFXTOFFG;                    // Clear XT1 fault flag
    SFRIFG1 &= ~OFIFG;
  }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag
  CSCTL0_H = 0;                             // Lock CS registers

  // Configure USCI_A0 for UART mode
  UCA0CTL1 |= UCSWRST;
  UCA0CTL1 = UCSSEL__ACLK;                  // Set ACLK = 32768 as UCBRCLK
  UCA0BR0 = 3;                              // 9600 baud
  UCA0MCTLW |= 0x5300;                      // 32768/9600 - INT(32768/9600)=0.41
                                            // UCBRSx value = 0x53 (See UG)
  UCA0BR1 = 0;
  UCA0CTL1 &= ~UCSWRST;                     // release from reset
  UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt

  while (1) {
    if (j == 5) {
        while(!(UCA0IFG & UCTXIFG));
        UCA0TXBUF = X_current;
        while(!(UCA0IFG & UCTXIFG));
        UCA0TXBUF = Y_current;
        while(!(UCA0IFG & UCTXIFG));
        UCA0TXBUF = Z_current;
        while(!(UCA0IFG & UCTXIFG));
        UCA0TXBUF = X_temp;
        while(!(UCA0IFG & UCTXIFG));
        UCA0TXBUF = Y_temp;
        while(!(UCA0IFG & UCTXIFG));
        UCA0TXBUF = Z_temp;
        j = 0;

    }

    ADC12CTL0 &= ~ADC12ENC;   // Toggeling ENC to restart sequence, else have to use
    ADC12CTL0 |= ADC12ENC;    // CONCEQ_3
    ADC12CTL0 |= ADC12SC;     // Sampling and conversion start

    __bis_SR_register(LPM0_bits | GIE);     // Enter LPM0, interrupts enabled
    __no_operation();
  }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_A0_VECTOR             // eUSCI ISR
__interrupt void USCI_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(UCA0IV,USCI_UART_UCTXCPTIFG))
    {
      case USCI_NONE: break;
      case USCI_UART_UCRXIFG:
          switch (j) {
           case 0:
               X_mag = UCA0RXBUF;
               j++;
               break;
           case 1:
               Y_mag = UCA0RXBUF;
               j++;
               break;
           case 2:
               Z_mag = UCA0RXBUF;
               j++;
               break;
           case 3:
               X_mag = UCA0RXBUF;
               j++;
               break;
           case 4:
               Y_mag = UCA0RXBUF;
               j++;
               break;
           case 5:
               Z_mag = UCA0RXBUF;
               break;
          }
        __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0 on reti
        break;
      case USCI_UART_UCTXIFG: break;
      case USCI_UART_UCSTTIFG: break;
      case USCI_UART_UCTXCPTIFG: break;
    }
  // Here we clear the buffer that has the data from the RPi and we store it in RXData
  if(X_mag == 0 && Y_mag == 0 && Z_mag == 0) {
      P3OUT |= BIT0;
      P3OUT |= BIT1;
      P2OUT &= ~BIT3;
  }
  if (X_mag > 100) {
      X_mag = 256 - X_mag;
      P3OUT &= ~BIT0;
  }
  if (Y_mag > 100) {
      Y_mag = 256 - Y_mag;
      P3OUT &= ~BIT1;
  }
  if (Z_mag > 100) {
      Z_mag = 256 - Z_mag;
      P2OUT |= BIT3;
  }
  TB0CCR5 = X_mag;
  TB0CCR6 = Y_mag;
  TB0CCR2 = Z_mag;

  __bic_SR_register_on_exit(CPUOFF);        // Exit LPM0 on reti
}

#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void)
{

  switch(__even_in_range(ADC12IV, ADC12IV_ADC12RDYIFG))
  {
    case ADC12IV_NONE:        break;        // Vector  0:  No interrupt
    case ADC12IV_ADC12OVIFG:  break;        // Vector  2:  ADC12MEMx Overflow
    case ADC12IV_ADC12TOVIFG: break;        // Vector  4:  Conversion time overflow
    case ADC12IV_ADC12HIIFG:  break;        // Vector  6:  ADC12BHI
    case ADC12IV_ADC12LOIFG:  break;        // Vector  8:  ADC12BLO
    case ADC12IV_ADC12INIFG:  break;        // Vector 10:  ADC12BIN
    case ADC12IV_ADC12IFG0:   break;        // Vector 12:  ADC12MEM0
    case ADC12IV_ADC12IFG1:   break;        // Vector 14:  ADC12MEM1
    case ADC12IV_ADC12IFG2:   break;        // Vector 16:  ADC12MEM2
    case ADC12IV_ADC12IFG3:   break;        // Vector 18:  ADC12MEM3
    case ADC12IV_ADC12IFG4:   break;        // Vector 20:  ADC12MEM4
    case ADC12IV_ADC12IFG5:
        X_temp = (int) ADC12MEM0/2;//(int) ((ADC12MEM0 * 3.3)/4096)*1000;
        Y_temp = (int) ADC12MEM1/2;//(int) ((ADC12MEM1 * 3.3)/4096)*1000;
        Z_temp = (int) ADC12MEM2/2;//(int) ((ADC12MEM2 * 3.3)/4096)*1000;
        X_current = (int) (ADC12MEM3/2);
        Y_current = (int) (ADC12MEM4/2);
        Z_current = (int) (ADC12MEM5/2);
        __bic_SR_register_on_exit(LPM4_bits);
        break;        // Vector 22:  ADC12MEM5
    case ADC12IV_ADC12IFG6:   break;        // Vector 24:  ADC12MEM6
    case ADC12IV_ADC12IFG7:   break;        // Vector 26:  ADC12MEM7
    case ADC12IV_ADC12IFG8:   break;        // Vector 28:  ADC12MEM8
    case ADC12IV_ADC12IFG9:   break;        // Vector 30:  ADC12MEM9
    case ADC12IV_ADC12IFG10:  break;        // Vector 32:  ADC12MEM10
    case ADC12IV_ADC12IFG11:  break;        // Vector 34:  ADC12MEM11
    case ADC12IV_ADC12IFG12:  break;        // Vector 36:  ADC12MEM12
    case ADC12IV_ADC12IFG13:  break;        // Vector 38:  ADC12MEM13
    case ADC12IV_ADC12IFG14:  break;        // Vector 40:  ADC12MEM14
    case ADC12IV_ADC12IFG15:  break;        // Vector 42:  ADC12MEM15
    case ADC12IV_ADC12IFG16:  break;        // Vector 44:  ADC12MEM16
    case ADC12IV_ADC12IFG17:  break;        // Vector 46:  ADC12MEM17
    case ADC12IV_ADC12IFG18:  break;        // Vector 48:  ADC12MEM18
    case ADC12IV_ADC12IFG19:  break;        // Vector 50:  ADC12MEM19
    case ADC12IV_ADC12IFG20:  break;        // Vector 52:  ADC12MEM20
    case ADC12IV_ADC12IFG21:  break;        // Vector 54:  ADC12MEM21
    case ADC12IV_ADC12IFG22:  break;        // Vector 56:  ADC12MEM22
    case ADC12IV_ADC12IFG23:  break;        // Vector 58:  ADC12MEM23
    case ADC12IV_ADC12IFG24:  break;        // Vector 60:  ADC12MEM24
    case ADC12IV_ADC12IFG25:  break;        // Vector 62:  ADC12MEM25
    case ADC12IV_ADC12IFG26:  break;        // Vector 64:  ADC12MEM26
    case ADC12IV_ADC12IFG27:  break;        // Vector 66:  ADC12MEM27
    case ADC12IV_ADC12IFG28:  break;        // Vector 68:  ADC12MEM28
    case ADC12IV_ADC12IFG29:  break;        // Vector 70:  ADC12MEM29
    case ADC12IV_ADC12IFG30:  break;        // Vector 72:  ADC12MEM30
    case ADC12IV_ADC12IFG31:  break;        // Vector 74:  ADC12MEM31
    case ADC12IV_ADC12RDYIFG: break;        // Vector 76:  ADC12RDY
    default: break;
  }

}
