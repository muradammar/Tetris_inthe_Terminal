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
int cur_tetrominoe_x_idx;       //tracks column position of current tetrominoe
int cur_tetrominoe_y_idx;       //tracks row position of current tetrominoe
int orientation;                //tracks orientation of the tetromenoe
int score = 0;              

//function declarations
int poll_input(char* input);
void place_tetrominoe();
void clear_full_lines();
void draw_game();
void apply_gravity();
void is_game_over();
void rotate_clockwise();
void move_left();
void move_right();
void draw_tetrominoe();
bool valid_position(int x_idx, int y_idx, int ori); //pass vars so position can be tested before changing position

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

    double time_since_update; //used to apply gravity in real time

    clock_t start_sec, end_sec;
    start_sec = clock(); //start timing

    place_tetrominoe();
    draw_game();

    //game loop
    while (!game_over) {

        //update elapsed time
        end_sec = clock();
        time_since_update = ((double) (end_sec - start_sec)) / CLOCKS_PER_SEC;


        if (time_since_update > 0.75) {
            
            //apply gravity 
            if (valid_position(cur_tetrominoe_x_idx, cur_tetrominoe_y_idx + 1, orientation)) {
                apply_gravity();

            //if floored, clear any full lines 
            //then add a new tetrominoe 
            //then check for game_over
            } else {

                clear_full_lines();
                is_game_over();
                place_tetrominoe();
                
            }

            //update start time to current time and update screen
            start_sec = clock();
            draw_tetrominoe();
            draw_game();
        }

        //------INPUT HANDLING--------------
        char input;
        int key_read;
        key_read = poll_input(&input);

        //read key and make sure position is valid
        if (input == 'w' && valid_position(cur_tetrominoe_x_idx, cur_tetrominoe_y_idx, orientation + 1)) {
            rotate_clockwise();
            draw_tetrominoe();
            draw_game();
            input = 'l';
        } else if (input == 'a' && valid_position(cur_tetrominoe_x_idx - 1, cur_tetrominoe_y_idx, orientation)) {
            move_left();
            draw_tetrominoe();
            draw_game();
            input = 'l'; //debounce
        } else if (input == 'd' && valid_position(cur_tetrominoe_x_idx + 1, cur_tetrominoe_y_idx, orientation)) {
            move_right();
            draw_tetrominoe();
            draw_game();
            input = 'l'; //debounce
        } else if (input == 's' && valid_position(cur_tetrominoe_x_idx, cur_tetrominoe_y_idx + 1, orientation)) {
            apply_gravity();
            draw_tetrominoe();
            draw_game();
            input = 'l'; //debounce
        }

        Sleep(20);
    }

    printf("\nGAME OVER\n");
    return 0;
}

void draw_game() {

    //clear screen and reset cursor
    printf("\033[2J\033[H");

    //display score
    printf("SCORE: %d\n\n", score);

    //draw current game array
    for (int i=0 ; i<HEIGHT ; i++) {
        printf("| ");
        for (int j=0 ; j<WIDTH ; j++) {

            if (game_array[i][j] == 0) {
                printf(". ");
            } else {
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
    } else {
        //handle no input
        return 0;
    }
}

//clear any filled lines and update the game array
void clear_full_lines () {

    bool full_line; 
    
    //start checking lines from the bottom
    for (int i=HEIGHT-1 ; i>=0 ; i--) {

        full_line = true; //assume true until proven otherwise
        for (int j=0 ; j<WIDTH ; j++) {
            
            if(game_array[i][j] == 0) {
                full_line = false;
                break; //stop checking line
            }
        }

        if (full_line) {

            //increment score
            score += 100;

            //shift all rows above cleared line (except for top row) down by one
            for(int row=i ; row>0 ; row--) {
                for(int j=0 ; j<WIDTH ; j++) {
                    game_array[i][j] = game_array[i - 1][j];
                }
            }
            //empty the top row
            for (int i=0 ; i<WIDTH; i++) {
                game_array[0][i] = 0;
            }
        }
    }
}

void apply_gravity() {

    //erase previous position of tetrominoe
    for (int i=cur_tetrominoe_y_idx ; i<cur_tetrominoe_y_idx+4 ; i++) {
        for (int j=cur_tetrominoe_x_idx; j<cur_tetrominoe_x_idx+4 ; j++) {

            if(tetrominoes[orientation][tetrominoe_idx][ ((i - cur_tetrominoe_y_idx) * 4) + (j - cur_tetrominoe_x_idx) ] == 1) {
                game_array[i][j] = 0;
            }    
        }
    }

    //move down one
    cur_tetrominoe_y_idx += 1;
}

void rotate_clockwise() {

    //erase previous position of tetrominoe
    for (int i=cur_tetrominoe_y_idx ; i<cur_tetrominoe_y_idx+4 ; i++) {
        for (int j=cur_tetrominoe_x_idx; j<cur_tetrominoe_x_idx+4 ; j++) {
            if(tetrominoes[orientation][tetrominoe_idx][ ((i - cur_tetrominoe_y_idx) * 4) + (j - cur_tetrominoe_x_idx) ] == 1) {
                game_array[i][j] = 0;
            }     
        }
    }
    orientation = (orientation + 1) % 4;
}

void move_left() {
    //erase previous position of tetrominoe
    for (int i=cur_tetrominoe_y_idx ; i<cur_tetrominoe_y_idx+4 ; i++) {
        for (int j=cur_tetrominoe_x_idx; j<cur_tetrominoe_x_idx+4 ; j++) {

            if(tetrominoes[orientation][tetrominoe_idx][ ((i - cur_tetrominoe_y_idx) * 4) + (j - cur_tetrominoe_x_idx) ] == 1) {
                game_array[i][j] = 0;
            }    
        }
    }

    cur_tetrominoe_x_idx -= 1;
}

void move_right() {
    //erase previous position of tetrominoe
    for (int i=cur_tetrominoe_y_idx ; i<cur_tetrominoe_y_idx+4 ; i++) {
        for (int j=cur_tetrominoe_x_idx; j<cur_tetrominoe_x_idx+4 ; j++) {

            if(tetrominoes[orientation][tetrominoe_idx][ ((i - cur_tetrominoe_y_idx) * 4) + (j - cur_tetrominoe_x_idx) ] == 1) {
                game_array[i][j] = 0;
            }    
        }
    }

    cur_tetrominoe_x_idx += 1;
}

void draw_tetrominoe() {

    for (int i=cur_tetrominoe_y_idx ; i<cur_tetrominoe_y_idx+4 ; i++) {
        for (int j=cur_tetrominoe_x_idx; j<cur_tetrominoe_x_idx+4 ; j++) {
            if (tetrominoes[orientation][tetrominoe_idx][ ((i-cur_tetrominoe_y_idx) * 4) + ((j-cur_tetrominoe_x_idx)) ] == 1) {
                game_array[i][j] = 1;
            }
        }
    }
}

//erase tetrominoe -> check new position -> redraw tetrominoe and return bool
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
                    //redraw the OG tetrominoe and return
                    draw_tetrominoe();
                    return false;
                }

                //check for collision with existing blocks
                if(game_array[i][j] == 1) {
                    //redraw the OG tetrominoe and return
                    draw_tetrominoe();
                    return false;
                }
            }            
        }
    }
    //redraw the OG tetrominoe and return
    draw_tetrominoe();
    return true;
}

void is_game_over() {

    bool top_line_reached = false;
    
    for (int i=0 ; i<WIDTH ; i++) {
        if (game_array[0][i] == 1) {
            top_line_reached = true;
        }
    }

    if (top_line_reached) game_over = true;
}