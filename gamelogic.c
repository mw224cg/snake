//____________________________________________ GAME LOGIC _____________________________________________________

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


//__________________________________ VGA Functionality  ____________________________________________________________

//----------------------- constants ----------------------------------------------
#define VGA_BUFFR        ((volatile unsigned int*) 0x04000100)
#define VGA_BACKBUFFR    ((volatile unsigned int*) 0x04000104)
#define VGA_STATCTRL     ((volatile unsigned int*) 0x0400010C) //Bit1 = swap status (0 == done)

#define SCREEN_WIDTH   320
#define SCREEN_HEIGHT   240
#define SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)

#define VGA_FB0   ((volatile unsigned char*) 0x08000000)
#define VGA_FB1   ((volatile unsigned char*) (0x08000000 + SCREEN_SIZE))

// --------------------- Global variables ----------------------------------------
volatile unsigned char *front_buffer = VGA_FB0; //pointer to first frame
volatile unsigned char *back_buffer  = VGA_FB1; // pointer to second frame


// ---------------------- functions ---------------------------------------------
void swap_buffers() { //fungerar
    while ((*VGA_STATCTRL & 0x1) == 1); //Vänta tills tidigare Swap är klar

    // Sätt backbuffer-adressen
    *VGA_BACKBUFFR = (unsigned int)back_buffer;
    *VGA_BUFFR = 0; // trigga swap

    // Byt pekare
    volatile unsigned char *tmp = front_buffer;
    front_buffer = back_buffer;
    back_buffer = tmp;
}

void clear_buffer(volatile unsigned char *buf) { //fungerar
    for (int i = 0; i < SCREEN_SIZE; i++) {
        buf[i] = 0;
    }
}

//____________________________________ Graphics logic ____________________________________________

//----------------------------- Constants ------------------------------------

#define RED 0xE0
#define GREEN 0x1C
#define BLUE 0x03
#define BLACK 0x00
#define WHITE 0xFF
#define CYAN 0x1F

//--------------------------- Functions ---------------------------------------

// Plots a px at coordinate (x,y) of specified color to the back buffer
// int x: (0 - 319)
// int y: (0 - 239)
void plot_pixel(int x, int y, unsigned char color){ //fungerar
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    int offset = y * SCREEN_WIDTH + x;
    back_buffer[offset] = color;
}

// Draws a 10*10 square at the specified row/column to the backbuffer
void draw_tile(int row, int column, unsigned char color){ //fungerar
    int start_x = column * TILE_SIZE;
    int start_y = row * TILE_SIZE;

    for (int y = 0; y < TILE_SIZE; y++) {
        for (int x = 0; x < TILE_SIZE; x++) {
            plot_pixel(start_x + x, start_y + y, color);
        }
    }
}

// Draws the whole matrix to the backbuffer
void draw_game_matrix(){
    for(int row = 0; row < ROWS; row++){
        for(int col = 0; col < COLUMNS; col++){
            unsigned char color = CYAN;
            switch (game_matrix[row][col]){
            case EMPTY: color = CYAN; break;
            case HEAD: color = RED; break;
            case BODY: color = BLACK; break;
            case TAIL: color = GREEN; break;
            case APPLE: color = WHITE; break;
            }

            draw_tile(row, col, color);
        }
    }
}

//___________________________________ INTERRUPTS_________________________________________________________

extern void enable_interrupt(void);

//------------------------- CONTSTANTS ---------------------------------------------------

// Timer addresses, each register is 16 bits, pointers to 2byte (short).
#define TIMER_STATUS_ADDR ((volatile unsigned short*) 0x04000020) //Timer status address Bit 0: TO, Bit 1: RUN
#define TIMER_CONTROL_ADDR ((volatile unsigned short*) 0x04000024) /*Control: Bit 0: ITO, Bit 1: CONT, Bit 2: START, Bit 3: STOP*/
#define TIMER_PERIOD_LOW_ADDR ((volatile unsigned short*) 0x04000028) //Timer period low address [0-15]
#define TIMER_PERIOD_HIGH_ADDR ((volatile unsigned short*) 0x0400002C) //Timer period high address [16-31]

// BTN addresses, each register is 32 bits, pointers to 4byte (int)
#define BTN_BASE_ADDR ((volatile int*) 0x040000D0)
#define BTN_INTERRUPT_MASK ((volatile int*) 0x040000D8) //+2 word offset
#define BTN_INTERRUPT_STATUS ((volatile int*)0x040000DC) //+3 word offset

//Constants
#define PERIOD ((unsigned int) 3000000 - 1) // Timer period i klockcykler (3 miljoner cykler = 0.1s vid 30MHz klocka)
#define TIMER_INTERRUPT 16
#define BUTTON_INTERRUPT 18

int timeoutcount = 0;


void timer_init(void){
    *TIMER_PERIOD_LOW_ADDR = PERIOD & 0xFFFF; //Bit 0-15 av perioden
    *TIMER_PERIOD_HIGH_ADDR = (PERIOD >> 16) & 0xFFFF; //Bit 16-31 av perioden
    
    *TIMER_CONTROL_ADDR = 0b0111;
    // Bit 0: ITO=1 (interrupt enable)
    // Bit 1: CONT=1 (continuous mode)
    // Bit 2: START=1 (start timer)
    // Bit 3: STOP=0 
}

void btn_init(){
    *BTN_INTERRUPT_MASK = 0x1;      //enable interrupts för BTN1
    *BTN_INTERRUPT_STATUS = 0x1;    //Nollställ IRQ-flagga
}

void interupt_init(void){
    timer_init();
    btn_init();
    enable_interrupt();
}

void timer_interrupt(){
    *TIMER_STATUS_ADDR = 0; //Nollställ IRQ

    timeoutcount++;

        if (timeoutcount >= 5) { //0,5s update
            timeoutcount = 0;             // Reset counter
            
            //------spellogik----
            move_snake();
            eat_apple();
            detect_self_collision();
            detect_wall_collision();

            if(collision == 0){
                write_snake_to_game_frame();
                render_frame();
                swap_buffers();
            }
            return;
        }
}

void btn_interrupt(){
    int button = *BTN_BASE_ADDR & 0x1;
    *BTN_INTERRUPT_STATUS = 0; //Nollställ IRQ

    if(button == 0){
        change_direction();
    }

}

void handle_interrupt(unsigned int cause) {
    switch (cause)
    {
    case 16: timer_interrupt(); break;
    case 18: btn_interrupt(); break;
    default: break;
    }
}


//______________________________ MAIN & start up ___________________________________________________

void start_game(){
    init_snake();
    new_apple();
    write_snake_to_matrix();

    clear_buffer((volatile unsigned char*) back_buffer);
    draw_game_matrix();
    swap_buffers();

    interupt_init();
}

int main(void){
    start_game();
    while(1);
}





