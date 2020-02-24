#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h" 

int getsw( void )
{

    return (PORTD >> 8) & 0x000F; //Shiftar 8 bitar till höger och maskerar resten

}

int getbtns(void) {

	return (PORTD >> 5) & 0x0007; //Shiftar bitarna 5 steg till höger och maskerar resten
}

int getbtn1(void)
{
    return (PORTF >> 1) & 0x01;   //Masks all bits except bit 1
}