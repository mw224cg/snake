#include "rand.h"
#include "image_start.h"
#include "image_game_over.h"

extern void print(const char*);
extern void print_dec(unsigned int);
extern void display_string(char*);
extern void time2string(char*,int);
extern void tick(int*);
extern void delay(int);
extern int nextprime( int );
extern void enable_interrupt(void);

// --------------------------- Constants ----------------------------------
#define TILE_SIZE 10
#define ROWS 24
#define COLUMNS 32
#define SNAKE_MAX_LENGTH 1000
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
int snake_length;
int should_grow = 0; // Flagga för att indikera om ormen ska växa
int collision = 0; // Flagga för kollision
Direction direction = UP;

//-------------------------------- Functions --------------------------------------------------------

//skapa en orm med riktning upp i mitten av skärmen (12,16) == HEAD, fungerar
void init_snake(){ 

    snake_length = INITIAL_SNAKE_LENGTH;
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

// Returnerar antal ätna äpplen.
int calculate_score(){
    int current_score = snake_length - INITIAL_SNAKE_LENGTH;
    return current_score;
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

    int index = random() % empty_count; // Välj en slumpmässig tom position
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
    if(head_position.row < 0 || head_position.row >= ROWS || head_position.column < 0 || head_position.column >= COLUMNS){
        collision = 1;
    }
}

void detect_collision_self(){ //fungerar
    Position head_position = snake[0].pos;
    for(int i = 1; i < snake_length; i++){
        if(head_position.row == snake[i].pos.row && head_position.column == snake[i].pos.column){
            collision = 1;
        }

    }
}

// If head == apple then grow the snake and generate a new apple
void eat_apple(){
    Position head_pos = snake[0].pos;
    if(head_pos.row == apple_position.row && head_pos.column == apple_position.column) {
        grow_snake();
        new_apple();
    }
}

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

//Displays the backbuffer immage
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

void image_to_VGA(unsigned char *image){
     for(int i = 0; i < SCREEN_SIZE; i++) {
        back_buffer[i] = image[i];
    }
    print("Image start/game_over\n");
    swap_buffers();
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
//___________________________ SCORE: 7-Segment Displays _____________________________________________
#define DISPLAY_BASE 0x04000050
#define DISPLAY_OFFSET 0x10

//Lista över värden för 7-segmentsdisplayen. 0 = LED ON
static const unsigned char display_values[12] = {
  0b01000000, // 0
  0b01111001, // 1
  0b00100100, // 2
  0b00110000, // 3
  0b00011001, // 4 
  0b00010010, // 5 
  0b00000010, // 6 
  0b01111000, // 7
  0b00000000, // 8
  0b00010000, // 9
  0b01111111, // '.'
  0b11111111  // ' '

  };

/* Sätter värdet på 7-segmentsdisplayen.
Inparameter: Display nummer (0-5) och värdet som ska visas (0-9 eller 10 för punkt).
Utparameter: void
*/
void set_displays(int display_number, int value){
volatile unsigned char* display_address = (volatile unsigned char*)(DISPLAY_BASE + display_number * DISPLAY_OFFSET); //8bit ptr to the display address
*display_address = display_values[value];
}

void display_score(){
    int score = calculate_score();

    int ones = score % 10;
    int tens = (score / 10) % 10;
    int hundreds = (score / 100) % 10;

    set_displays(0, ones);
    set_displays(1, tens);
    set_displays(2, hundreds);

    set_displays(3, 11);
    set_displays(4, 11);
    set_displays(5, 11);
}

void reset_score(){
    print("Reset displays\n");
    set_displays(0, 0);
    set_displays(1, 0);
    set_displays(2, 0);

    set_displays(3, 11);
    set_displays(4, 11);
    set_displays(5, 11);
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

//------------------------ Globala variabler ------------------------------------------------

volatile int timeoutcount = 0;
volatile int tick_flag = 0;


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

//returns value of btn
int get_btn(){
    if(*BTN_BASE_ADDR & 0x1 == 1){
        print("BTN press\n");
        return *BTN_BASE_ADDR & 0x1;
    } else{
        return 0;
    }
    
}

void interupt_init(void){
    timer_init();
    btn_init();
    enable_interrupt();
}

void disable_timer_interrupt(){
    *TIMER_CONTROL_ADDR = 0b010;
    *TIMER_STATUS_ADDR = 0x1;      // Rensa endast timerflagga
    print("Timer interrupt disabled\n");
}
    *TIMER_STATUS_ADDR = 0x1;      // Rensa endast timerflagga
    print("Timer interrupt disabled\n");
}

void disable_btn_interrupt(){
    *BTN_INTERRUPT_MASK = 0x0;
    *BTN_INTERRUPT_STATUS = 0x1;
    print("Disabled btn int\n");
}

void disable_interrupts(){
    disable_btn_interrupt();
    disable_timer_interrupt();
}
/* Every 0.3s:
move the snake,
check if apple and head matches
detect collisions

*/
void timer_interrupt(){
    *TIMER_STATUS_ADDR = 0; //Nollställ IRQ

    timeoutcount++;

        if (timeoutcount >= 3) { //0,3s update
            timeoutcount = 0;             // Reset counter
            
            tick_flag = 1;
        }
}

void btn_interrupt(){
    int button = *BTN_BASE_ADDR & 0x1;
    *BTN_INTERRUPT_STATUS = 0; //Nollställ IRQ

    if(button == 0){
        change_direction();
        print("BTN int\n");
    }

}

// Enkel dummy-implementation
void handle_interrupt(unsigned int cause) {
    switch (cause)
    {
    case 16: timer_interrupt(); break;
    case 18: btn_interrupt(); break;
    default: break;
    }
}

//___________________________________ MENU & SCREENS ________________________________________________________


//______________________________ MAIN & start up ___________________________________________________

// Nollställ gv, initiera orm + äpple, skriv objekt till positioner på spelplanen, rita spelplanen och visa på VGA
// tillåt interrupts
void start_game(){
    collision = 0;
    direction = UP;
    tick_flag = 0;
    reset_score();
    init_snake();
    new_apple();
    write_snake_to_matrix();

    clear_buffer(back_buffer);
    draw_game_matrix();
    swap_buffers();

    interupt_init();
}



int main(void){
    while(1){
        image_to_VGA(image_start);

        while (get_btn() == 0);

        start_game();

        while (collision == 0){
            if(tick_flag){
                tick_flag = 0;

                move_snake();
                detect_collision_self();
                detect_collision_wall();

                if(collision) break;

                eat_apple();
                display_score();
                write_snake_to_matrix();
                draw_game_matrix();
                swap_buffers();
            }
        }

        image_to_VGA(image_game_over);
        disable_interrupts();

        print("GAME OVER\n");
        print("Final score: ");
        print_dec(((unsigned int)calculate_score()));
        print("\nTo play again: press BTN1");

        while (get_btn()==0);
    }
}
