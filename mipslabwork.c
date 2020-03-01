/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdio.h>
#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <stdlib.h>
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "mipslab.h" /* Declatations for these labs */
#include <string.h>

#define TMR2PERIOD ((80000000 / 256) / 10)

char textstring[] = "text, more text, and even more text!";
int timeoutcountX = 0;
int timeoutcountO = 0;
int mytime = 0x0000;
int hsPoints[2];

int xTimer = 0xFF;
int oTimer = 0xFF;

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
	//REMOVE LATER
	char* gay = name[2];
	display_string(0, gay);
	///
	display_update();
	screen = 1;
}

int writeHiScore(void)
{
	screen = 4;
	clearScreen();

	//Show who won
	display_string(0, "Winner:     P: "); //Base
	if (win == 1) //X-win
	{
		textbuffer[0][8] = 88; //X
		timeLeft(xTimer, 0, 14, 1);
	}
	else if (win == 2) //O-win
	{
		textbuffer[0][8] = 79; //O
		timeLeft(oTimer, 0, 14, 1);
	}
	display_string(1, "Enter name:");

	textbuffer[2][0] = 65; //A
	display_string(3, "Back press BTN 1");
	display_update();
}

int hiScore(void)
{
	clearScreen();
	display_string(0, "  High Score  ");

	textbuffer[1][0] = 49; //"1"
	textbuffer[1][1] = 46; //"."

	textbuffer[2][0] = 50; //"2"
	textbuffer[2][1] = 46; //"."

	//Show names
	int i;
	char sc[2] = "";
	if(nameScore[0] != 0){
		for (i = 2; i < 5; i++)
		{
			textbuffer[1][i] = name[0][i-2]; //Display name
			itoa(nameScore[0], sc);	
			textbuffer[1][6] = sc[0]; //Score
			textbuffer[1][7] = sc[1]; //Score
		}
	}
	if(nameScore[1] != 0){
		for (i = 2; i < 5; i++)
		{
			textbuffer[2][i] = name[1][i-2]; //Display name
			itoa(nameScore[1], sc);	
			textbuffer[2][6] = sc[0]; //Score
			textbuffer[2][7] = sc[1]; //Score
		}
	}

	
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
	createCursor();
	display_update();
}
/* Interrupt Service Routine */
void user_isr(void)
{
	//check flag
	if (IFS(0) & 0x100)
	{
		IFS(0) = 0; //clear flag
		if (timerStart == 1)
		{
			if (turn == 1)
			{
				PORTE = xTimer;
				timeoutcountX++;
				if (timeoutcountX == 40)
				{ 
					timeoutcountX = 0;
					xTimer -= 0x1;
					xTimer = (xTimer / 2);
					PORTE = xTimer;
				}
				if (xTimer == 0x0)
				{
					win = 2;
					timerStart = 0;
					initWin();
				}
			}
			if (turn == 2)
			{
				PORTE = oTimer;
				timeoutcountO++;
				if (timeoutcountO == 40)
				{
					timeoutcountO = 0;
					oTimer -= 0x1;
					oTimer = (oTimer / 2);
					PORTE = oTimer;
				}
				if (oTimer == 0x0)
				{
					win = 1;
					timerStart = 0;
					initWin();
				}
			}
		}
	}
	return;
}

/* Lab-specific initialization goes here */
void labinit(void)
{

	TRISECLR = 0xFF; // Set first 8 bits to zero (sets them as output pins for LED)

	TRISFCLR = 0x1; //Initialize BTN 1
	// Initialize port D, set bits 11-5 as inputs. SW1-4 & BTN2-4
	TRISD = TRISD & 0x0fe0;

	PR2 = TMR2PERIOD;
	T2CON = 0x0;	   // clearing the clock
	T2CONSET = 0x70;   // setting the prescale
	TMR2 = 0;		   // reset timer to 0
	T2CONSET = 0x8000; // turn timer on, set bit 15 to 1

	// configuring the priority level
	IPC(2) = 7;
	// enabling bit 8 for the interupt
	IEC(0) = (1 << 8);

	// calling interupt from labwork.S
	enable_interrupt();

	return;
}

/* This function is called repetitively from the main program */
void labwork(void)
{
	int sw = getsw();
	int btn = getbtns();
	int btn1 = getbtn1();

	/*			GLOBAL Buttons 			*/
	if ((btn1 & 1)) //BTN 1
	{
		menu(); //Back to menu
		resetGame(); //Resets game
	}

	/*			Menu Buttons 		*/
	else if ((btn & 1) && screen == 0) //BTN 2
	{
		board(); //Start Game
		delay(500);
		screen = 3;
	}

	else if ((btn & 2) && screen == 0) //BTN 3
	{
		hiScore();
	}

	else if ((btn & 4) && screen == 0) //BTN 4
	{
		instr();
	}

	/*			Game Buttons			*/
	else if ((btn & 1) && screen == 3) //BTN 2
	{
		moveCursor(2);
	}
	else if ((btn & 4) && screen == 3) //BTN 4
	{
		moveCursor(1);
	}

	else if ((btn & 2) && screen == 3) //BTN 3
	{
		place();
	}

	/*		Write High Score Buttons		*/

	else if (((btn & 4) && screen == 4) && letter > 65) //BTN 4, Bokstav bak
	{
		letter--;
		textbuffer[2][initials] = letter;
		display_update();
		delay(250);
	}

	else if (((btn & 1) && screen == 4) && letter < 90) //BTN 2, Bokstav fram
	{
		letter++;
		textbuffer[2][initials] = letter;
		display_update();
		delay(250);
	}

	else if (((btn & 2) && screen == 4) && initials < 3) //BTN 3
	{
		//writeToTemp(initials, letter);
		name[2][initials] = letter;
		//tempName[initials] = letter;
		textbuffer[2][initials] = letter;
		if(initials != 2){
			textbuffer[2][initials+1] = 65;
		}
		initials++;
		display_update();
		delay(250);
		if(initials == 3)
		{
			textbuffer[2][initials] = 0;
			resetGame();
			delay(250);
			menu();
		}
		letter = 65;
	}
}
