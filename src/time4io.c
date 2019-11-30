#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

int getsw( void ){
    int sw = PORTD;
    return (sw >> 8) & 0xf;
}

int getbtns( void ){
    int bt = PORTD;
    return (bt >> 5) & 0x7;
}