/*
Game logic goes here.
*/


#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"



struct Pacman{
    int x_pos[2];
    int y_pos[2];
    int facing;
    int x_mov;
    int y_mov; 
};

struct Pacman pc = {{31, 32}, {10, 11}, 0, 0, 0};

struct Ghost{
    int x_pos;
    int y_pos;
    int facing;
    int x_mov;
    int y_mov;
};

struct Ghost gh1 = {25, }


uint8_t translator(uint8_t point){
    switch(point) {
        case 0:
            return 0x3;
        case 1:
            return 0x0;
    }
    return 0;
}

void direction(){
    if(PORTD&0x80){ //Left BTN4
        pc.y_mov = 0;
        pc.x_mov = -1;
    }
    if(PORTD&0x40){ //Right BTN3
        pc.y_mov = 0;
        pc.x_mov = 1;
    }
    if(PORTD&0x20){ //Down BTN2
        pc.y_mov = 1;
        pc.x_mov = 0;
    }
    if(PORTF&0x2){ //Up BTN1
        pc.y_mov = -1;
        pc.x_mov = 0;
    }
}

void move(const uint8_t level[][64]){
    if(bound_check(pc, level)){
        pc.x_pos[0] += pc.x_mov;
        pc.x_pos[1] += pc.x_mov;
        pc.y_pos[0] += pc.y_mov;
        pc.y_pos[1] += pc.y_mov;
    }
}

void tick(const uint8_t level[][64]){
    move(level);
    uint8_t img[512];
    render(level_1, img);
    display_screen(img);
}

int bound_check(struct Pacman pc, const uint8_t level[][64]){
    if((level[pc.y_pos[0]+pc.y_mov][pc.x_pos[0]+pc.x_mov] == 1) ||
        (level[pc.y_pos[0]+pc.y_mov][pc.x_pos[1]+pc.x_mov] == 1) ||
        (level[pc.y_pos[1]+pc.y_mov][pc.x_pos[0]+pc.x_mov] == 1) ||
        (level[pc.y_pos[1]+pc.y_mov][pc.x_pos[1]+pc.x_mov] == 1)){
        return 0;
    }
}


void render(const uint8_t level[][64], uint8_t img[]){
    int i, j;
    uint8_t point_data;

    for(i = 0; i < 16; i++){
        for(j = 0; j < 64; j++){
            uint8_t point = level[i][j];
            point_data = translator(point);
            if(i%4 == 0) {img[j*2+(128*(i/4))] = point_data; img[j*2+(128*(i/4))+1] = point_data;}
            else{
                img[j*2+(128*(i/4))] = img[j*2+(128*(i/4))] + (point_data<<(2*(i%4)));
                img[j*2+(128*(i/4))+1] = img[j*2+(128*(i/4))+1] + (point_data<<(2*(i%4)));
            }
        }
    }

    img[pc.x_pos[0]*2+(128*(pc.y_pos[0]/4))+1] -= (0x1<<(2*(pc.y_pos[0]%4))+1);
    img[pc.x_pos[1]*2+(128*(pc.y_pos[0]/4))] -= (0x1<<(2*(pc.y_pos[0]%4))+1);
    img[pc.x_pos[0]*2+(128*(pc.y_pos[1]/4))+1] -= (0x1<<(2*(pc.y_pos[1]%4)));
    img[pc.x_pos[1]*2+(128*(pc.y_pos[1]/4))] -= (0x1<<(2*(pc.y_pos[1]%4)));
    //img[pc.x_pos[1]*2+(128*(pc.y_pos[0]/4))] -= (0x3<<(2*(pc.y_pos[0]%4)));

}
