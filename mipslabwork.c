/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */
#define TMR2PERIOD ((80000000 / 256) / 10)

char textstring[] = "text, more text, and even more text!";
int timeoutcount = 0;
int mytime = 0x0000;

//Project variables
int screen = 0; // Menu = 0, Instr = 1, Hi-Score = 2, Game = 3;
int Xturn = 0;
int Oturn = 0;
int win = 0;
char boardArr[9];

char marker = 0x178;

// defining the int pointer, trise, volatile because you 
//don't want the c compiler to optimise
volatile int * portE = (volatile int *) 0xbf886110;

  
// Set *E to address of TRISE.
volatile int *E = (volatile int *) 0xbf886100;


int menu(void)
{
	//display_string(0, "   Tic-Tac-Toe   ");
	display_string(0, "" + marker);
	
	display_string(1, "BTN 4: HowToPlay");
	display_string(2, "BTN 3: Hi-Score");
	display_string(3, "BTN 2: Start!");
	display_update();
	screen = 0;
}

int instr(void)
{
	display_string(0, "Instructions");
	display_string(1, "BTN 4: Left");
	display_string(2, "BTN 3: Confirm");
	display_string(3, "BTN 2: Right");
	display_update();
}

int board(void)
{

	display_string(0, "Begin!" );
    display_string(1, "|O|X|X|  |X Turn|");
    display_string(2, "|O|X|O|  X: 2p");
    display_string(3, "|X|O|X|  O: 1p");
	display_update();

}
/* Interrupt Service Routine */
void user_isr( void ) {

  // check flag
  if(IFS(0) & 0x100){
    // clearing flag
    IFS(0) = 0;
    timeoutcount++;

    if (timeoutcount == 10){
     time2string( textstring, mytime );
 	// display_string( 0, textstring );
     display_update();
      tick( &mytime );
      timeoutcount = 0;
    }
  }
    
  // code for counting LED
  if(IFS(0) & 0x80){
    // clearing flag
    IFS(0) = 0;
    // for LEDs ticking
    * portE =  * portE + 0x1;
    // only for the last 8 bits
    * E = * E & 0xFF00;
  }

  return;
}

/* Lab-specific initialization goes here */
void labinit( void )
{

	// Set first 8 bits to zero, i.e. sets them as output pins. LEDS
	*E = *E & 0xff00;

	// Initialize port D, set bits 11-5 as inputs. SW1-4 & BTN1-4
	TRISD = TRISD & 0x0fe0;

	PR2 = TMR2PERIOD;
	T2CON = 0x0; // clearing the clock
	T2CONSET = 0x70; // setting the prescale
	TMR2 = 0; // reset timer to 0
	T2CONSET = 0x8000; // turn timer on, set bit 15 to 1

	// configuring the priority level
	IPC(2) = 7;
	// enabling bit 8 for the interupt
	IEC(0) = 0x100;

  // calling interupt from labwork.S
	enable_interrupt();

  return;
}

/* This function is called repetitively from the main program */
void labwork( void )
{
	int sw = getsw();
	int btn = getbtns();

	if (btn & 1) //BTN 2
	{
		board();
		mytime = 0x0000;
	}

	if (btn & 2) //BTN 3
	{

	}

	if (btn & 4) //BTN 4
	{
		instr();
	}


//  delay( 1000 );
// time2string( textstring, mytime );
 // display_string( 3, textstring );
 // display_update();
 // tick( &mytime );
 // display_image(96, icon);
}
