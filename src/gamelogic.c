/*
Game logic goes here.
*/


#include <stdint.h>
#include <stdlib.h>
#include <pic32mx.h>
#include "mipslab.h"

void *stdout = (void *) 0;
int candiesYeeted;
int amtCandies1 = 104;
int amtCandies2 = 118;

// Ratman stuct :B
struct Ratman{
    int x_pos[2];
    int y_pos[2];
    int facing;
    int x_mov;
    int y_mov;
    int prev_x_mov;
    int prev_y_mov;
};

struct Ratman pc;

// Struct 
struct Ghost{
    int x_pos[2];
    int y_pos[2];
    int facing;
    int x_mov;
    int y_mov;
    int prev_pos[2];
};

// Make spooky ghosts.
struct Ghost gh1, gh2, gh3, gh4, gh5, gh6, gh7, gh8, gh9;

// Add place the ghosts' pointers in the array 'ghosts'.
struct Ghost* ghosts[9] = {&gh1, &gh2, &gh3, &gh4, &gh5, &gh6, &gh7, &gh8, &gh9};

int num_ghost; // How many ghosts do we use, dependent on chosen difficulty.

int current_level; // Current level.

int game_state; // Game state. 0 = Normal. 1 = Win. -1 = Gameover.

uint8_t current_candy[16][64]; // Copy of current level's candy data.

// Translate point data to render data for render_board.
uint8_t translator(uint8_t point){
    switch(point) {
        case 0:
            return 0x3;
        case 1:
            return 0x0;
    }
    return 0;
}

// Check that new input direction is not the same as last input.
int prev_direction_check(int y, int x){
    if((y==pc.y_mov) && (x==pc.x_mov)){
        return 0;
    }
    return 1;
}

// Change player characters movement, save previous movement.
void direction(){
    if(PORTD&0x80){ //Left BTN4
        if(prev_direction_check(0, -1)){
            pc.prev_y_mov = pc.y_mov;
            pc.prev_x_mov = pc.x_mov;
        }
        pc.y_mov = 0;
        pc.x_mov = -1;
    }
    if(PORTD&0x40){ //Right BTN3
        if(prev_direction_check(0, 1)){
            pc.prev_y_mov = pc.y_mov;
            pc.prev_x_mov = pc.x_mov;
        }
        pc.y_mov = 0;
        pc.x_mov = 1;
    }
    if(PORTD&0x20){ //Down BTN2
        if(prev_direction_check(1, 0)){
            pc.prev_y_mov = pc.y_mov;
            pc.prev_x_mov = pc.x_mov;
        }
        pc.y_mov = 1;
        pc.x_mov = 0;
    }
    if(PORTF&0x2){ //Up BTN1
        if(prev_direction_check(-1, 0)){
            pc.prev_y_mov = pc.y_mov;
            pc.prev_x_mov = pc.x_mov;
        }
        pc.y_mov = -1;
        pc.x_mov = 0;
    }
}

// Check that player new movement direction does not put player out of bounds.
int bound_check_pc(const uint8_t level[][64]){
    int x_0 = pc.x_pos[0] + pc.x_mov;
    int x_1 = x_0 + 1;
    int y_0 = pc.y_pos[0] + pc.y_mov;
    int y_1 = y_0 + 1;
    if((level[y_0][x_0] == 1) ||
        (level[y_0][x_1] == 1) ||
        (level[y_1][x_0] == 1) ||
        (level[y_1][x_1] == 1)){
        return 0;
    }
}

// Check that player previous movement direction does not put player out of bounds. 
int bound_check_pc_prev(const uint8_t level[][64]){
    if(((-pc.x_mov) == pc.prev_x_mov) && ((-pc.y_mov) == pc.prev_y_mov)){
        return 0;
    }
    int x_0 = pc.x_pos[0] + pc.prev_x_mov;
    int x_1 = x_0 + 1;
    int y_0 = pc.y_pos[0] + pc.prev_y_mov;
    int y_1 = y_0 + 1;
    if((level[y_0][x_0] == 1) ||
        (level[y_0][x_1] == 1) ||
        (level[y_1][x_0] == 1) ||
        (level[y_1][x_1] == 1)){
        return 0;
    }
}

// Set facing depending on what way player character moves.
void tail_set(int y, int x){
    if((y == 0) && (x == -1)){ // Facing left
        pc.facing = 0;
    } else if((y == 0) && (x == 1)){ // Facing right
        pc.facing = 1;
    } else if((y == -1) && (x == 0)){ // Facing up
        pc.facing = 2;
    } else if((y == 1) && (x == 0)){ // Facing down
        pc.facing = 3;
    }
}

// Move in new direction if possible, otherwise move in previous direction if possible,
// otherwise stand still.
void move(const uint8_t level[][64]){
    if(bound_check_pc(level)){
        tail_set(pc.y_mov, pc.x_mov);
        pc.x_pos[0] += pc.x_mov;
        pc.x_pos[1] += pc.x_mov;
        pc.y_pos[0] += pc.y_mov;
        pc.y_pos[1] += pc.y_mov;
        pc.prev_x_mov = 0;
        pc.prev_y_mov = 0;
    } else if(bound_check_pc_prev(level)){
        tail_set(pc.prev_y_mov, pc.prev_x_mov);
        pc.x_pos[0] += pc.prev_x_mov;
        pc.x_pos[1] += pc.prev_x_mov;
        pc.y_pos[0] += pc.prev_y_mov;
        pc.y_pos[1] += pc.prev_y_mov;
        pc.y_mov = pc.prev_y_mov;
        pc.x_mov = pc.prev_x_mov;
        pc.prev_x_mov = 0;
        pc.prev_y_mov = 0;
    }
}

// Check if given ghost is about to walk into a different ghost, return 1 if so, else 0.
int ghost_check(int gn, int arr[2]){
    int x_0 = ghosts[gn]->x_pos[0] + arr[0];
    int x_1 = x_0 + 1;
    int y_0 = ghosts[gn]->y_pos[0] + arr[1];
    int y_1 = y_0 + 1;
    int i;
    for(i = 0; i < num_ghost; i++){
        if(i == gn){
            i += 1;
            continue;
        }
        return ((ghosts[i]->x_pos[0] == x_0 && ghosts[i]->y_pos[0] == y_0) || 
                (ghosts[i]->x_pos[0] == x_0 && ghosts[i]->y_pos[1] == y_1) ||
                (ghosts[i]->x_pos[1] == x_1 && ghosts[i]->y_pos[0] == y_0) ||
                (ghosts[i]->x_pos[1] == x_1 && ghosts[i]->y_pos[1] == y_1));
    }
    return 0;
}

// Check so that given ghost number gn does not make illegal move.
int bound_check_gh(const uint8_t level[][64], int gn, int arr[2]){
    int x_0 = ghosts[gn]->x_pos[0] + arr[0];
    int x_1 = x_0 + 1;
    int y_0 = ghosts[gn]->y_pos[0] + arr[1];
    int y_1 = y_0 + 1;
    if((level[y_0][x_0] == 1) ||
        (level[y_1][x_0] == 1) ||
        (level[y_0][x_1] == 1) ||
        (level[y_1][x_1] == 1) ||
        ((ghosts[gn]->prev_pos[0] == x_0) && 
        (ghosts[gn]->prev_pos[1] == y_0)) ||
        ghost_check(gn, arr)){ // Check not go prev pos
        return 0;
    }
    ghosts[gn]->prev_pos[0] = ghosts[gn]->x_pos[0];
    ghosts[gn]->prev_pos[1] = ghosts[gn]->y_pos[0];
    ghosts[gn]->x_pos[0] += arr[0];
    ghosts[gn]->x_pos[1] += arr[0];
    ghosts[gn]->y_pos[0] += arr[1];
    ghosts[gn]->y_pos[1] += arr[1];
    return 1;
}

// Places ghost at its previous position.
void go_back(int gn){
    ghosts[gn]->y_pos[0] = ghosts[gn]->prev_pos[1];
    ghosts[gn]->y_pos[1] = ghosts[gn]->prev_pos[1] + 1;
    ghosts[gn]->x_pos[0] = ghosts[gn]->prev_pos[0];
    ghosts[gn]->x_pos[1] = ghosts[gn]->prev_pos[0] + 1;
}

// Iteratives over active ghosts, dependent on difficulty, and moves them randomly.
// They are not allowed to go back, unless they roll a new leval move within ten tries.
void move_ghost_rn(const uint8_t level[][64]){
    int move_arr[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    int rng;
    int i;
    int stuck;
    for(i = 0; i < num_ghost; i++){
        rng = rand();
        stuck = 0;
        while (!bound_check_gh(level, i, move_arr[(rng+i)%4])){
            if(stuck == 10){
                go_back(i);
            }
            stuck += 1;
            rng = rand();
        }
    }
}

// Reset game static variables and position of things.
void reset(){
    // Reset
    game_state = 0;
    candiesYeeted = 0;

    // Reset Ratman
    pc = (struct Ratman) {{31, 32}, {10, 11}, 0, 0, 0, 0, 0};
    gh1 = (struct Ghost) {{25, 26}, {1, 2}, 0, 0, 0, {25, 1}};
    gh2 = (struct Ghost) {{20, 21}, {1, 2}, 0, 0, 0, {20, 4}};
    gh3 = (struct Ghost) {{40, 41}, {1, 2}, 0, 0, 0, {40, 4}};
    gh4 = (struct Ghost) {{1, 2}, {7, 8}, 0, 0, 0, {1, 7}};
    gh5 = (struct Ghost) {{1, 2}, {4, 5}, 0, 0, 0, {1, 4}};
    gh6 = (struct Ghost) {{61, 62}, {4, 5}, 0, 0, 0, {61, 4}};
    gh7 = (struct Ghost) {{1, 2}, {7, 8}, 0, 0, 0, {1, 7}};
    gh8 = (struct Ghost) {{61, 62}, {7, 8}, 0, 0, 0, {61, 7}};
    gh9 = (struct Ghost) {{61, 62}, {13, 14}, 0, 0, 0, {61, 13}};
}

// Check gameover with. Gameover occurs when player character touches one 
// of the active ghosts.
void gameover(){
    int x_0 = pc.x_pos[0];
    int x_1 = pc.x_pos[1];
    int y_0 = pc.y_pos[0];
    int y_1 = pc.y_pos[1];
    int ghx_0, ghx_1, ghy_0, ghy_1, i;
    for(i = 0; i < num_ghost; i++){
        ghx_0 = ghosts[i]->x_pos[0];
        ghx_1 = ghosts[i]->x_pos[1];
        ghy_0 = ghosts[i]->y_pos[0];
        ghy_1 = ghosts[i]->y_pos[1];
        if((x_0 == ghx_1 && y_0 == ghy_0) || //PUL GUR
            (x_0 == ghx_0 && y_0 == ghy_0) || 
            (x_0 == ghx_0 && y_0 == ghy_0) || 
            (x_0 == ghx_0 && y_1 == ghy_0) ||
            (x_0 == ghx_0 && y_0 == ghy_0) ||
            (x_1 == ghx_1 && y_0 == ghy_1) ||
            (x_1 == ghx_1 && y_0 == ghy_1) ||
            (x_1 == ghx_0 && y_1 == ghy_0)){
                game_state = -1;
            }
    }
}

// Checks if player character is on candy, if so remove and increment 'candiesYeeted'.
void eat(uint8_t candy[][64]) {
    if (candy[pc.y_pos[0]][pc.x_pos[0]] == 2 ||  candy[pc.y_pos[0]][pc.x_pos[0]] == 3) {
        candy[pc.y_pos[0]][pc.x_pos[0]] = 0;
        candiesYeeted++;
    }
    if (candy[pc.y_pos[1]][pc.x_pos[0]] == 2 || candy[pc.y_pos[1]][pc.x_pos[0]] == 3) {
        candy[pc.y_pos[1]][pc.x_pos[0]] = 0;
        candiesYeeted++;
    }
    if (candy[pc.y_pos[1]][pc.x_pos[1]] == 2 || candy[pc.y_pos[1]][pc.x_pos[1]] == 3) {
        candy[pc.y_pos[1]][pc.x_pos[1]] = 0;
        candiesYeeted++;
    }
    if (candy[pc.y_pos[0]][pc.x_pos[1]] == 2 || candy[pc.y_pos[0]][pc.x_pos[1]] == 3) {
        candy[pc.y_pos[0]][pc.x_pos[1]] = 0;
        candiesYeeted++;
    }
}

// When 'candiesYeeted' has been incremented sufficiently (all candies of given level
// have been eaten), player wins given level, current_state is set to 1.
void win(){
    if((current_level == 1) && (candiesYeeted == amtCandies1)){
        game_state = 1;
    } else if((current_level == 2) && (candiesYeeted == amtCandies2)){
        game_state = 1;
    }
}

// Perform all required steps to simulate game. :)
int tickgame(const uint8_t level[][64], uint8_t candy[][64], int num_ghost){
    move(level);
    eat(candy);
    move_ghost_rn(level);
    render(level, candy);
    gameover();
    win();
}

// Initiate difficulty .
void initdiff(int diff_lvl){
    switch(diff_lvl){
        case 1:
            num_ghost = 3;
            break;
        case 2:
            num_ghost = 4;
            break;
        case 3:
            num_ghost = 5;
            break;
        case 4:
            num_ghost = 6;
    }   
}

// Copy candy data from candydata.c to current_candy.
void copy_candy(uint8_t candy[][64], uint8_t copy[][64]){
    int i, j;
    for(i=0; i<16; i++){
        for(j= 0; j < 64; j++){
            copy[i][j] = candy[i][j];
        }
    }
}

//Init new level, sets current_level and copies candy for said level, calls reset().
void initlvl(int lvl){
    switch(lvl){
        case 1:
            current_level = 1;
            copy_candy(candy_1, current_candy);
            reset();
            break;
        case 2:
            current_level = 2;
            copy_candy(candy_2, current_candy);
            reset();
            break;
    }
}

// Calls tick game with different levels depending on current level.
// Returns game state.
int nextstate(){
    switch(current_level){
        case 1:
            tickgame(level_1, current_candy, num_ghost);
            return game_state;
        case 2:
            tickgame(level_2, current_candy, num_ghost);
            return game_state;
    }
}

void render_board(const uint8_t level[][64], uint8_t img[]);
void render_candy(uint8_t candy[][64], uint8_t img[]);
void progress(uint8_t img[]);
void render_ghosts(uint8_t img[]);
void render_Ratman(uint8_t img[]);

// Render it all.
void render(const uint8_t level[][64], uint8_t candy[][64]){
    uint8_t img[512];
    render_board(level, img);
    progress(img);
    render_Ratman(img);
    render_ghosts(img);
    render_candy(candy, img);
    display_screen(img);
}

// Render board.
void render_board(const uint8_t level[][64], uint8_t img[]){
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
}

// Render level information only, useful for information.
void render_screen(const uint8_t level[][64]){
    uint8_t img[512];
    render_board(level, img);
    display_screen(img);
}

// Render candy.
void render_candy(uint8_t candy[][64], uint8_t img[]){
    int i, j;
    for(i = 0; i < 16; i++){
        for(j = 0; j < 64; j++){
            uint8_t point = candy[i][j];
            if (point == 2) {
                img[j*2+(128*(i/4))] -= (0x1<<(2*(i%4))+1);
            } else if (point == 3) {
                img[j*2+(128*(i/4))] -= (0x1<<(2*(i%4)));
            }
        }
    }    
}

// Render progress bar.
void progress(uint8_t img[]) {
    int pro;
    int i = 0;
    if (current_level == 1) {
        pro = (38*candiesYeeted/amtCandies1);
        for (i; i< pro; i++) {
            img[22*2+128+i+1] -= (0x1<<(7));
            img[22*2+(128*2)+i+1] -= (0x1);
        }   
    } else if (current_level == 2) {
        pro = (61*candiesYeeted/amtCandies2);
        for (i; i< pro; i++) {
            img[3+i*2+128*3] -= (0x3<<(5));
            img[4+i*2+128*3] -= (0x3<<(5));
        }
    }
    
}

// Render ghosts.
void render_ghosts(uint8_t img[]){
    int i;
    for(i = 0; i < num_ghost; i++){
        img[ghosts[i]->x_pos[0]*2+(128*(ghosts[i]->y_pos[0]/4)+1)] -= (0x1<<(2*(ghosts[i]->y_pos[0]%4))+1);
        img[ghosts[i]->x_pos[0]*2+(128*(ghosts[i]->y_pos[1]/4)+1)] -= (0x1<<(2*(ghosts[i]->y_pos[1]%4)));
        img[ghosts[i]->x_pos[0]*2+(128*(ghosts[i]->y_pos[0]/4))+2] -= (0x1<<(2*(ghosts[i]->y_pos[0]%4))+1);
    }     
}

// Render Ratman (and his tail).
void render_Ratman(uint8_t img[]){
    img[pc.x_pos[0]*2+(128*(pc.y_pos[0]/4))+1] -= (0x1<<(2*(pc.y_pos[0]%4))+1);
    img[pc.x_pos[1]*2+(128*(pc.y_pos[0]/4))] -= (0x1<<(2*(pc.y_pos[0]%4))+1);
    img[pc.x_pos[0]*2+(128*(pc.y_pos[1]/4))+1] -= (0x1<<(2*(pc.y_pos[1]%4)));
    img[pc.x_pos[1]*2+(128*(pc.y_pos[1]/4))] -= (0x1<<(2*(pc.y_pos[1]%4)));

    switch(pc.facing){
        case 0:
            img[pc.x_pos[1]*2+(128*(pc.y_pos[1]/4))+1] -= (0x1<<(2*(pc.y_pos[1]%4)));
            break;
        case 1:
            img[pc.x_pos[0]*2+(128*(pc.y_pos[1]/4))] -= (0x1<<(2*(pc.y_pos[1]%4)));
            break;
        case 2:
            img[pc.x_pos[0]*2+(128*((pc.y_pos[1])/4))+1] -= (0x1<<(2*((pc.y_pos[1])%4)+1));
            break;
        case 3:
            img[pc.x_pos[1]*2+(128*((pc.y_pos[0])/4))] -= (0x1<<(2*((pc.y_pos[0])%4)));
            break;
    }
}