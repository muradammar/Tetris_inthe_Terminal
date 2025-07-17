#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>

//game dimensions
#define WIDTH 10
#define HEIGHT 20

//tetrominoe dimensions
#define T_WIDTH 4
#define T_HEIGHT 4

//global vars
char game_array[HEIGHT][WIDTH] = {{0}};
bool game_over = false;
int tetrominoe_idx;             //used to choose one of the 7 tetrominoes
double time_since_update;       //used to apply gravity in real time
int cur_tetrominoe_x_idx;       //tracks column position of current tetrominoe
int cur_tetrominoe_y_idx;       //tracks row position of current tetrominoe
int orientation;                //tracks orientation of the tetromenoe

//function declarations
int poll_input(char* input);
void place_tetrominoe();
int line_cleared();
void draw_game();
void apply_gravity();
void rotate_left();
void rotate_right();
void draw_tetrominoe();
bool valid_position(int x_idx, int y_idx, int ori); //pass vars so position can be tested before changing position
void temporary_draw(int x_idx, int y_idx, int ori); //helper function for testing if position is valid

//[orientation][tetromenoe_type][actual_tetromenoe_shape]
char tetrominoes[4][7][T_WIDTH * T_HEIGHT] = 
{
    //----------- 0 deg clkwise --------------------
    {
    {0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //O
    {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0}, //I
    {0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //S
    {1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //Z
    {0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0}, //L
    {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0}, //J
    {0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, //T
    },
    //----------- 90 deg clkwise --------------------
    {
    {0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //O
    {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0}, //I
    {0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0}, //S
    {0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, //Z
    {1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, //L
    {0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0}, //J
    {0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //T
    },
    //----------- 180 deg clkwise -------------------
    {
    {0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //O
    {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0}, //I
    {0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //S
    {1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //Z
    {0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //L
    {1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //J
    {0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0}, //T
    },
    //----------- 270 deg clkwise -------------------
    {
    {0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //O
    {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0}, //I
    {0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0}, //S
    {0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, //Z
    {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0}, //L
    {0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, //J
    {0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0}, //T
    },
};


int main() {

    clock_t start_sec, end_sec;
    start_sec = clock(); //start timing

    place_tetrominoe();
    draw_game();

    //game loop
    while (!game_over) {

        //update elapsed time
        end_sec = clock();
        time_since_update = ((double) (end_sec - start_sec)) / CLOCKS_PER_SEC;


        if (time_since_update > 1) {
            //apply_gravity

            if (valid_position(cur_tetrominoe_x_idx, cur_tetrominoe_y_idx + 1, orientation)) {
                apply_gravity();
            }

            else {
                place_tetrominoe();
            }

            

            //update start time to current time
            start_sec = clock();

            draw_tetrominoe();
            draw_game();
            // printf("%d\n", cur_tetrominoe_y_idx);
            // Sleep(500);
        }


        //place_tetrominoe();
        
        char input;
        int key_read;
        key_read = poll_input(&input);

        if (input == 'a') {
            rotate_left();
            draw_game();
            input = 'n';
        }

        if (input == 'd') {
            rotate_right();
            draw_game();
            input = 'n';
        }

        Sleep(20);
        //printf("\e[1;1H\e[2J"); //clear terminal
        
        //system("cls");
    }

    return 0;
}

void draw_game() {

    //clear screen and reset cursor
    printf("\033[2J\033[H");

    //draw new content
    for (int i=0 ; i<HEIGHT ; i++) {
        printf("| ");
        for (int j=0 ; j<WIDTH ; j++) {

            if (game_array[i][j] == 0) {
                printf(". ");
            }

            else {
                printf("# ");
            }
        }
        printf("|\n");
    }
}

//randomly chooses a tetromenoe (tetromenoe_idx) and resets its position to the top (cur_tetromenoe_idx_x/y)
//should be called everytime the current tetrominoe hits the floor
void place_tetrominoe() {

        //reset tetrominoe position indexes and orientation
        cur_tetrominoe_x_idx = 3;
        cur_tetrominoe_y_idx = 0;
        orientation = 0;


    //     //TEMPORARY BLOCK
    //     for (int i=0 ; i<4 ; i++) {
    //         for (int j=3; j<WIDTH-3 ; j++) {
    //         // printf("%d", tetrominoes[tetrominoe_idx][(i*4) + ((j - 3))]);
    //         // Sleep(1000);

    //         game_array[i][j] = 0;
            
    //     }
    // }

    tetrominoe_idx = rand() % 7;

    //spawn chosen tetrominoe on top
    for (int i=cur_tetrominoe_y_idx ; i<cur_tetrominoe_y_idx+4 ; i++) {
        for (int j=cur_tetrominoe_x_idx; j<cur_tetrominoe_x_idx+4 ; j++) {
            // printf("%d", tetrominoes[tetrominoe_idx][(i*4) + ((j - 3))]);
            // Sleep(1000);
            if (tetrominoes[orientation][tetrominoe_idx][ (i*4) + ((j - 3)) ] == 1) {
                game_array[i][j] = 1;
            }
        }
    }
}

int poll_input(char* input) {
    
    //assign user input
    if(_kbhit()) {
        *input = _getch();
        return 1;
    }

    else {
        //handle no input
        return 0;
    }
}

//return the row index of a cleared line else return -1
int line_cleared () {

}

void apply_gravity() {

    //erase previous position of tetrominoe
    for (int i=cur_tetrominoe_y_idx ; i<cur_tetrominoe_y_idx+4 ; i++) {
        for (int j=cur_tetrominoe_x_idx; j<cur_tetrominoe_x_idx+4 ; j++) {
            game_array[i][j] = 0;
        }
    }

    //move down one
    cur_tetrominoe_y_idx += 1;
}

void rotate_left() {

    //erase previous position of tetrominoe
    for (int i=cur_tetrominoe_y_idx ; i<cur_tetrominoe_y_idx+4 ; i++) {
        for (int j=cur_tetrominoe_x_idx; j<cur_tetrominoe_x_idx+4 ; j++) {
            game_array[i][j] = 0;
        }
    }

    orientation = (orientation + 3) % 4;
}

void rotate_right() {

    //erase previous position of tetrominoe
    for (int i=cur_tetrominoe_y_idx ; i<cur_tetrominoe_y_idx+4 ; i++) {
        for (int j=cur_tetrominoe_x_idx; j<cur_tetrominoe_x_idx+4 ; j++) {
            game_array[i][j] = 0;
        }
    }

    orientation = (orientation + 1) % 4;
}

void draw_tetrominoe() {

    for (int i=cur_tetrominoe_y_idx ; i<cur_tetrominoe_y_idx+4 ; i++) {
        for (int j=cur_tetrominoe_x_idx; j<cur_tetrominoe_x_idx+4 ; j++) {
            // printf("%d", tetrominoes[tetrominoe_idx][(i*4) + ((j - 3))]);
            // Sleep(1000);
            if (tetrominoes[orientation][tetrominoe_idx][ ((i-cur_tetrominoe_y_idx) * 4) + ((j-cur_tetrominoe_x_idx)) ] == 1) {
                game_array[i][j] = 1;
            }
        }
    }
}


bool valid_position(int x_idx, int y_idx, int ori) {

    //temporarily erase previous position of tetrominoe
    for (int i=cur_tetrominoe_y_idx ; i<cur_tetrominoe_y_idx+4 ; i++) {
        for (int j=cur_tetrominoe_x_idx; j<cur_tetrominoe_x_idx+4 ; j++) {

            if(tetrominoes[orientation][tetrominoe_idx][ ((i - cur_tetrominoe_y_idx) * 4) + (j - cur_tetrominoe_x_idx) ] == 1) {
                game_array[i][j] = 0;
            }
            
        }
    }

    //check given position for validity
    for (int i=y_idx ; i<y_idx+4 ; i++) {
        for (int j=x_idx; j<x_idx+4 ; j++) {
            
            //only check filled cells
            if(tetrominoes[ori][tetrominoe_idx][ ((i - y_idx) * 4) + (j - x_idx) ] == 1) {
                
                //check for out of bounds
                if(j < 0 || j >= WIDTH || i >= HEIGHT) {
                    return false;
                }

                //check for collision with existing blocks
                if(game_array[i][j] == 1) {
                    return false;
                }

            }            
            
        }
    }

    return true;

    //redraw the OG tetrominoe
    draw_tetrominoe();

}

//returns true if given tetrominoe position does not collide with walls or floor
//else returns false
// bool valid_position(int x_idx, int y_idx, int ori) {

//     //temporarily draw given position
//     temporary_draw(x_idx, y_idx, ori);

//     //check for valid positions
//     for (int i=x_idx ; i<x_idx+4 ; i++) {
//         for (int j=y_idx ; j<y_idx+4 ; j++) {

//             //only check cells that are "filled"
//             if (tetrominoes[ori][tetrominoe_idx][ ((i-y_idx) * 4) + (j-x_idx) ] == 1) {

//                 //check x position for out of bounds
//                 if (i < 0 || i >= WIDTH) {

//                     printf("1 ");

//                     //restore position
//                     temporary_draw(cur_tetrominoe_x_idx, cur_tetrominoe_y_idx, orientation);
//                     return false;
                    
//                 }

//                 //check y position for touching floor
//                 if (j >= HEIGHT) {

//                     printf("2 ");

//                     //restore position
//                     temporary_draw(cur_tetrominoe_x_idx, cur_tetrominoe_y_idx, orientation);
//                     return false;
                    
//                 }

//                 //check y position for touching other blocks
//                 if (game_array[i][j] == 1) {

//                     printf("3 ");

//                     //restore position
//                     temporary_draw(cur_tetrominoe_x_idx, cur_tetrominoe_y_idx, orientation);
//                     return false;
                    
//                 }
//             }
            
//         }
//     }

//     //restore position
//     for (int i=cur_tetrominoe_y_idx ; i<cur_tetrominoe_y_idx+4 ; i++) {
//         for (int j=cur_tetrominoe_x_idx; j<cur_tetrominoe_x_idx+4 ; j++) {
//             game_array[i][j] = 0;
//         }
//     }

//     temporary_draw(cur_tetrominoe_x_idx, cur_tetrominoe_y_idx, orientation);
//     return true;
// }

// void temporary_draw(int x_idx, int y_idx, int ori) {

//     //temporarily draw given position
//     //erase previous position of tetrominoe
//     for (int i=y_idx ; i<y_idx+4 ; i++) {
//         for (int j=x_idx; j<x_idx+4 ; j++) {
//             game_array[i][j] = 0;
//         }
//     }

//     //draw new position
//     for (int i=y_idx ; i<y_idx+4 ; i++) {
//         for (int j=x_idx; j<x_idx+4 ; j++) {
//             // printf("%d", tetrominoes[tetrominoe_idx][(i*4) + ((j - 3))]);
//             // Sleep(1000);
//             if (tetrominoes[ori][tetrominoe_idx][ ((i-y_idx) * 4) + ((j-x_idx)) ] == 1) {
//                 game_array[i][j] = 1;
//             }
//         }
//     }

// }