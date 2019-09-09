#include "utils.h"
#include "ili9341.h"
#include "ili9341gfx.h"

/* DRAW FUNCTIONS */
void draw_line(int x0, int y0, int x1, int y1, int colour);
void draw_circle_helper(int xc, int yc, int x, int y, uint16_t colour);
void draw_circle(int xc, int yc, int r, uint16_t colour);
void draw_square(uint16_t x, uint16_t y, uint16_t l, uint16_t colour);
void draw_walls(Wall_Coord walls[], uint8_t walls_no, uint16_t colour);
void draw_margins(uint16_t colour);

/* SETUP FUNCTIONS */
void LCD_setup();
Circle init_ball(uint16_t radius, uint16_t colour);
Circle get_finish_point(uint16_t radius, uint16_t colour);
void generate_fst_walls(Wall_Coord walls[], uint16_t walls_no);

/* GAME PHYSICS FUNCTIONS */
int check_finish(Circle finish, Coord ball, uint16_t radius);
Coord check_collisions_easy(Coord old_pos, Coord new_pos, uint16_t radius, Wall_Coord walls[], uint16_t walls_no);
int check_collisions_hard(Coord old_pos, Coord new_pos, uint16_t radius, Wall_Coord walls[], uint16_t walls_no);