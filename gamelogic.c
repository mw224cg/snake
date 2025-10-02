#include "rand.h"
// --------------------------- Constants ----------------------------------
#define TILE_SIZE 10
#define ROWS 24
#define COLUMNS 32
#define SNAKE_MAX_LENGTH 100
#define INITIAL_SNAKE_LENGTH 3


//------------------------------ Structs ----------------------------------
typedef enum{
    EMPTY,
    HEAD,
    BODY,
    TAIL,
    APPLE
} Types;

typedef enum {
    UP,
    RIGHT,
    DOWN,
    LEFT,
} Direction;

typedef struct {
    int row;
    int column;
} Position;

typedef struct{
    Position pos;
    Types type;
} SnakeSegment;

// ---------------------- Global variables -----------------------------------------
Types game_matrix[24][32];  //representerar spelaplanen som ett rutnät av 10*10 px, 24 rader, 32 kolumner

Position apple_position;
SnakeSegment snake[SNAKE_MAX_LENGTH];
int snake_length = INITIAL_SNAKE_LENGTH; // Initial längd på ormen
int should_grow = 0; // Flagga för att indikera om ormen ska växa
int collision = 0; // Flagga för kollision
Direction direction = UP;

//skapa en orm med riktning upp i mitten av skärmen (12,16) == HEAD, fungerar
void init_snake(){ 
    int start_row = ROWS/2;
    int start_col = COLUMNS/2;

    snake[0] = (SnakeSegment){{start_row, start_col}, HEAD};
    snake[1] = (SnakeSegment){{start_row + 1, start_col}, BODY};
    snake[2] = (SnakeSegment){{start_row + 2, start_col}, TAIL};
}

void clear_matrix(){ //fungerar
    for(int row = 0; row < ROWS; row++){
        for(int col = 0; col < COLUMNS; col++){
            game_matrix[row][col] = EMPTY;
        }
    }
}

void write_apple_to_matrix(){ //fungerar
    int row = apple_position.row;
    int col = apple_position.column;
    game_matrix[row][col] = APPLE;
}

void write_snake_to_matrix(){ //fungerar
    clear_matrix();

    for(int i = 0; i < snake_length; i++){
        int row = snake[i].pos.row;
        int col = snake[i].pos.column;
        
        game_matrix[row][col] = snake[i].type;
    }
    write_apple_to_matrix();
}

void new_apple(){ //fungerar, new_apple måste skrivas efter init_snake

    Position empty_positions[COLUMNS * ROWS];
    int empty_count = 0;

    for(int row = 0; row < ROWS; row++){
        for(int col = 0; col < COLUMNS; col++){
            if(game_matrix[row][col] == EMPTY){
                empty_positions[empty_count].row = row;
                empty_positions[empty_count].column = col;
                empty_count++;
            }
        }
    }
    if(empty_count == 0) return; // Om inga tomma positioner finns, gör inget

    int index = rand() % empty_count; // Välj en slumpmässig tom position
    apple_position = empty_positions[index]; // Sätt äpplets position
}

void change_direction(){ //fungerar
    switch (direction)
    {
    case UP: direction = LEFT; break;
    case RIGHT: direction = UP; break;
    case DOWN: direction = RIGHT; break;
    case LEFT: direction = DOWN; break;
    }
}

void grow_snake(){ //fungerar
    should_grow = 1;
}

void move_snake(){ //fungerar
    Position new_head_pos = snake[0].pos;
    // Flytta huvudet ett steg i nuvarande riktning
    switch (direction) {
        case UP:    new_head_pos.row -= 1; break;
        case DOWN:  new_head_pos.row += 1; break;
        case LEFT:  new_head_pos.column -= 1; break;
        case RIGHT: new_head_pos.column += 1; break;
    }

    if(should_grow){
        snake_length++;
        should_grow = 0;
    }

    // Flytta varje segment till positionen av föregående segment, från snake_length-1 till 1
    for (int i = snake_length - 1; i > 0; i--){
        snake[i].pos = snake[i - 1].pos;
        snake[i].type = BODY;
    }

    // Uppdatera huvudets position och typ
    snake[0].pos = new_head_pos;
    snake[0].type = HEAD;
    // Uppdatera svansens typ
    snake[snake_length - 1].type = TAIL;
    
}

void detect_collision_wall(){
    Position head_position = snake[0].pos;
    if(head_position.row < 0 || head_position.row > ROWS || head_position.column < 0 || head_position.column > COLUMNS){
        collision = 1;
    }
}

void detect_collision_self(){ //fungerar
    Position head_position = snake[0].pos;
    for(int i = 0; i < snake_length; i++){
        if(head_position.row == snake[i].pos.row && head_position.column == snake[i].pos.column){
            collision = 1;
        }

    }
}

//____________________________________________________________________________________________________________



