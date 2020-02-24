/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdio.h>
#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */
#include <string.h> 

#define TMR2PERIOD ((80000000 / 256) / 10)

char textstring[] = "text, more text, and even more text!";
int timeoutcount = 0;
int mytime = 0x0000;



//Project variables
int screen = 0; // Menu = 0, Instr = 1, Hi-Score = 2, Game = 3;
int Xturn = 0;
int Oturn = 0;
int win = 0;
char boardArr[3][7] = {
	{124, 43, 124, 43, 124, 43, 124},
	{124, 43, 124, 43, 124, 43, 124},
	{124, 43, 124, 43, 124, 43, 124},
};
  
int menu(void)
{
	display_string(0, "   Tic-Tac-Toe   ");
	display_string(1, "BTN 4: HowToPlay");
	display_string(2, "BTN 3: Hi-Score");
	display_string(3, "BTN 2: Start!");
	display_update();
	screen = 0;
}

int instr(void)
{
	display_string(0, "BTN 4: Left");
	display_string(1, "BTN 3: Confirm");
	display_string(2, "BTN 2: Right");
	display_string(3, "Back press BTN 1");

	display_update();
	screen = 1;
}

int hiScore(void)
{
	display_string(0, "  High Score  ");
	display_string(1, "1. AHJ ");
	display_string(2, "2. AHN ");
	display_string(3, "Back press BTN 1");
	display_update();
	screen = 2;
}

int board(void)
{
	display_string(0, "  TicTacToe  ");
	display_string(1, "|-|-|-|");
    display_string(2, "|-|-|-|");
    display_string(3, "|-|-|-|");
	display_update();
	screen = 3;

	createCursor();
}

/* Interrupt Service Routine */
void user_isr( void ) {

  // check flag
  if(IFS(0) & 0x100){
	// clearing flag
	IFS(0) = 0;
	timeoutcount++;

	if (timeoutcount == 10){
	/*    time2string( textstring, mytime );
	// display_string( 0, textstring );
		display_update();
		tick( &mytime ); */
		timeoutcount = 0;
		PORTE =  PORTE + 0x1; //Incrementing leds by 1 every second
    }
  }
    
  // code for counting LED & Reseting when full
  if(IFS(0) & 0x80){
    // clearing flag
    IFS(0) = 0;
    // for LEDs ticking
    PORTE =  PORTE + 0x1;
    // only for the last 8 bits
	TRISECLR = 0xFF;
  }

  return;
}

/* Lab-specific initialization goes here */
void labinit( void )
{

	TRISECLR = 0xFF; // Set first 8 bits to zero (sets them as output pins for LED)

	TRISFCLR = 0x1;	//Initialize BTN 1
	// Initialize port D, set bits 11-5 as inputs. SW1-4 & BTN2-4
	TRISD = TRISD & 0x0fe0;

	PR2 = TMR2PERIOD;
	T2CON = 0x0; // clearing the clock
	T2CONSET = 0x70; // setting the prescale
	TMR2 = 0; // reset timer to 0
	T2CONSET = 0x8000; // turn timer on, set bit 15 to 1

	// configuring the priority level
	IPC(2) = 7;
	// enabling bit 8 for the interupt
	IEC(0) = (1<<8);

  // calling interupt from labwork.S
	enable_interrupt();

  return;
}

/* This function is called repetitively from the main program */
void labwork( void )
{
	int sw = getsw();
	int btn = getbtns();
	int btn1 = getbtn1();


/*
BTN 4: Left
BTN 3: Confirm
BTN 2: Right
*/

/*			Menu Buttons 		*/
	if ((btn & 1) && screen == 0) //BTN 2
	{
		board();	//Start Game
	}

	if ((btn & 2) && screen == 0) //BTN 3
	{
		hiScore();
	}

	if ((btn & 4) && screen == 0) //BTN 4
	{
		instr();
	}

/*			Instruction Buttons 		*/
	if ((btn1 & 1) && screen == 1) //BTN 1
	{
		menu();		//Back to menu
	}

/*			HiScore Buttons 		*/
	if ((btn1 & 1) && screen == 2) //BTN 1
	{
		menu();		//Back to menu
	}

/*			Game Buttons			*/
if ((btn & 1) && screen == 3)
	{
		moveCursor(2);
	}
if ((btn & 4) && screen == 3) //BTN 4
	{
		moveCursor(1);
	}

//  delay( 1000 );
// time2string( textstring, mytime );
 // display_string( 3, textstring );
 // display_update();
 // tick( &mytime );
 // display_image(96, icon);
}
