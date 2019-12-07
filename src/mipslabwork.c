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

int timeoutcount = 0;


/* Interrupt Service Routine */
void user_isr( void ){
  if(IFS(0)&0x8000){
    IFS(0) &= ~0x8000;
    PORTE++;
  }
  direction();
  if(IFS(0)&0x100){
    IFS(0) &= ~0x100;
    timeoutcount++;
  }
  if(timeoutcount==2){
    timeoutcount = 0;
    tick(level_1);
  }
}

/* Lab-specific initialization goes here */
void labinit( void ){
  //Init pacboi

  T2CON = 0x0;
  PR2 = 0x0;
  T2CON = 0x8070;
  PR2 = 31250;
  TRISD |= 0xE0;
  TRISF |= 0x2;
  IEC(0) |= 0x8100; // turn on T2IE to enable T2, and  interrupts
  IPC(3) = 0x1f000000; //set max priority and subpriority for SW3 interrupts
  IPC(2) = 0x1F;  // set max priority and subpriority for T2 interrupts
  enable_interrupt();
}

/* This function is called repetitively from the main program */
void labwork( void ){ 

}
