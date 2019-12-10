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
#include <stdlib.h>

int timeoutcount = 0;
int stage = 0;
int difficulty = 0;
int speed = 2;

void game_select_init();
void next();

/* Interrupt Service Routine 
   */
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
    if(timeoutcount==speed){
        timeoutcount = 0;
        int game_state = nextstate();
        next(game_state);
    }
}

/* Lab-specific initialization goes here */
void labinit( void ){
    srand((unsigned)231013);
    TRISD |= 0xE0; // Init BTN 4-2
    TRISF |= 0x2; // Init BTN 1
    int i = 1;
    render_screen(level_START); 
    while(i){ // Hold until button press
        if((PORTD&0x80) || (PORTD&0x40) || (PORTD&0x20) || (PORTF&0x2)){
            i--;
        }
    }
    quicksleep(3000000);
    game_select_init();
    T2CON = 0x0;
    PR2 = 0x0;
    T2CON = 0x8070;
    PR2 = 31250;
    
    IEC(0) |= 0x8100; // turn on T2IE to enable T2, and  interrupts
    IPC(3) = 0x1f000000; //set max priority and subpriority for SW3 interrupts
    IPC(2) = 0x1F;  // set max priority and subpriority for T2 interrupts
    enable_interrupt();    
}

// Start new game, select difficulty, display relevant information
void game_select_init(){
    stage = 0;
    render_screen(level_MENU);
    int i = 1;
    while(i){
        if(PORTD&0x80){
            speed = 2;
            difficulty = 1;
            i--;
            render_screen(level_EASY);
            quicksleep(7000000);
        } else if(PORTD&0x40){
            speed = 2;
            difficulty = 2;
            i--;
            render_screen(level_MEDIUM);
            quicksleep(7000000);
        } else if(PORTD&0x20){
            speed = 2;
            difficulty = 3;
            i--;
            render_screen(level_HARD);
            quicksleep(7000000);
        } else if(PORTF&0x2){
            speed = 1;
            difficulty = 4;
            i--;
            render_screen(level_GOD);
            quicksleep(7000000);
        }
    }
    render_screen(level_ARROWS);
    quicksleep(7000000);
    render_screen(level_1_INTRO);
    quicksleep(4000000);
    stage = 1;
    initdiff(difficulty);
    initlvl(stage);
}

// Check for changes in game state (win/gameover) and act appropriately.
void next(int gs){
    switch(gs){
            case -1:
                render_screen(level_OOF);
                quicksleep(7000000);
                game_select_init();
                break;
            case 1:
                stage++;
                initlvl(stage); // Next level, maybe have "NEXT" page
                if(stage == 2){
                    render_screen(level_2_INTRO);
                    quicksleep(7000000);
                }
                break;
    }
    if(stage == 3){
        render_screen(level_GG);
        quicksleep(7000000);
        game_select_init();
    }
}

/* This function is called repetitively from the main program */
void labwork( void ){ 

}
