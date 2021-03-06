/*
    Shutter Release Timer 430
    Copyright (C) 2016  Giovanni D'Italia

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <msp430.h>
#define T    2
char i = 0;     //Timer_A interrupts counter
char sec = 1;

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;     // Stop WDT

  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ;

  CCTL0 = CCIE;                             // CCR0 interrupt enabled
  TACTL = TASSEL_2 + MC_1 + ID_3;           // SMCLK/8, upmode
  CCR0 =  62500;                            // CCR0 at 2Hz = 0.5s


  P1DIR = 0x00;       // Shut down everything
  P1OUT = 0x00;

  P1REN |= BIT7;      // Enable internal pull-up resistors
  P1OUT |= BIT7;      // Select pull-up mode for P1.7
  P1IE  |= (BIT7);    // Enable interrupt for P1.7
  P1IES |= (BIT7);    // Falling Edge 1 -> 0
  P1IFG  &= ~BIT7;    // Clear interrupt flag for P1.7


  P1DIR = 0x7f;            // P1.0 and P1.6 pins output 1.7 are input
  P1OUT |= BIT0;
  //low-power mode
  _BIS_SR(LPM0_bits + GIE);          // Enter LPM0 w/ interrupt

  while(1)                          //Loop
    {
    }

    }

    // Timer A0 interrupt service routine
    void Timer_A (void) __attribute__((interrupt(TIMER0_A0_VECTOR)));
    void Timer_A (void)
    {
      i++;
      if(i == (sec*T)){     //every (sec*T) interrupts shut!
        P1OUT |= BIT6;
        __delay_cycles(100000);
        P1OUT &= ~BIT6;
        i = 0;              //reset Timer_A interrupts counter
      }

    }

    void Port_1 (void) __attribute__((interrupt(PORT1_VECTOR)));
    void Port_1 (void)
    {
      __delay_cycles(300000); // simple debounce

      //set shutter time and turn on corresponding led
      switch (sec) {
          case 1:
            P1OUT &= ~BIT0;
            sec = 3;
            P1OUT |= BIT1;
            break;
        case 3:
            P1OUT &= ~BIT1;
            sec = 5;
            P1OUT |= BIT2;
            break;
        case 5:
            P1OUT &= ~BIT2;
            sec = 15;
            P1OUT |= BIT3;
            break;
        case 15:
            P1OUT &= ~BIT3;
            sec = 30;
            P1OUT |= BIT4;
            break;
        case 30:
            P1OUT &= ~BIT4;
            sec = 1;
            P1OUT |= BIT0;
            break;
      }
      P1IFG &= (!BIT7);
    }
