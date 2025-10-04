// Test code used during development of the project


//__________________________DEBUG___________________________________
void read_game_frame(){
    for(int row = 0; row < 24; row++){
        for(int col = 0; col < 32; col++){
            if(game_matrix[row][col] != 0){
                print("position (row, col): ");
                print_dec(row);
                print(", ");
                print_dec(col);
                print(" value: ");
                print_dec(game_matrix[row][col]);
                print("\n");
            }
        }
    }
    print("-------------------------------\n");
}


int main(void){
//INIT : funk
  init_snake();
  new_apple();
  write_snake_to_matrix();
  read_game_frame();
  
  //FLYTTA :funk
  
  move_snake();
  write_snake_to_matrix();
  read_game_frame();
  
  //VÄX
  grow_snake();
  move_snake();
  write_snake_to_matrix();
  read_game_frame();
  
  //VÄX + BYT RIKTNING + 2 STEG : funk
  
  grow_snake();
  change_direction();
  move_snake();
  move_snake();
  write_snake_to_matrix();
  read_game_frame();
  
  //Test self coll:
  grow_snake();
  change_direction();
  move_snake();
  change_direction();
  grow_snake();
  move_snake();
  move_snake();
  write_snake_to_matrix();
  detect_collision_self();
  print("collision: ");
  print_dec(collision);
  
  //test wall coll
  
  // Test plot pixels & swap/clear buff

for(int y = 0; y < SCREEN_HEIGHT; y++){ // vertical line at x = 10 //BILD 1
    plot_pixel(10, y, WHITE);
}

for(int x = 0; x < SCREEN_WIDTH; x++){ // horizontal line at y = 10
    plot_pixel(x, 10, RED);
}
swap_buffers();

for(int y = 0; y < SCREEN_HEIGHT; y++){ // vertical line at x = 10 //BILD 2
    plot_pixel(310, y, WHITE);
}

for(int x = 0; x < SCREEN_WIDTH; x++){ // horizontal line at y = 10
    plot_pixel(x, 230, RED);
}

swap_buffers();

//CLEAR
clear_buffer(back_buffer);
swap_buffers();

//TEST draw TILE : FUNK
clear_buffer(back_buffer);
draw_tile(0,0,RED);
draw_tile(0,31,WHITE);
draw_tile(23,0,BLUE);
draw_tile(23,31,GREEN);

swap_buffers();

//test draw matrix : FUNK
clear_buffer(back_buffer);
draw_game_matrix();
swap_buffers();

clear_buffer(back_buffer);
move_snake();
write_snake_to_matrix();
draw_game_matrix();
delay(1000);
swap_buffers();

  
  }
