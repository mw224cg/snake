# snake

This is the mini-project for the course IS1200 using the DTEK-V board.

## Input / Output
The project uses interrupts from BTN1 and the timer for user input and updating the frames and the logical movement of objects in the game.
The program outputs the pictures of the game to VGA, by using a backbuffer to draw in while one is displayed.
The current score is displayed on the 7-segment displays

## Game logic
The map is represented by a 2D array, a matrix dividing the screen into squares of the size 10*10 pixels.
Each square in the matrix contains a value representing an object in the game (eg snake, apple, empty space).
The snake is represented by an array, where each position in the array contains the position and type of the segment in the snake.
Movement is based on the current direction of the snake, setting the position of the head 1 step forward and moving the rest of the
segments to the spot before it (eg. snake[2].pos = [snake[1].pos).

## Pictures
The start up / game-over screens where created by:
* Creating a 320*240 sized work area in GIMP
* Importing the chosen picture and scaling it to fit the work area
* Changing the mode to "indexed colors"
* --> Choosing the amount of colors to 8 bit == 256
* -->>Apply dithering to avoid color-bleed
* Export as .raw and converting the .raw-file to an array: unsigned char[] = { "8bit color value for (0,0)", "8bit color value for (1,0)", .... "8bit color value for (319,239)" }
* Each value (x,y) is then written to the VGA backbuffer ( (0,0) written to back_buffer[0] ... (319,239) written to back_buffer[76800])

## Comment
When first developing the program, weird bugs where met when the different functionalities where divided into different files.
Now almost all code is in the same file: game_logic.c, which is very messy but will do for now.
The code is very dependent on global variables, and some of the functions could be made more general which would make it easier to further develop the game.
