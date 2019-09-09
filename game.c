#include "game.h"

/* DRAW FUNCTIONS */

void draw_line(int x0, int y0, int x1, int y1, int colour)
{
    int dx, dy, p, x, y;
 
	dx = x1-x0;
	dy = y1-y0;
 
	x = x0;
	y = y0;
 
	p = 2 * dy - dx;
 
	while (x < x1)
	{
		if (p >= 0)
		{
			ili9341_drawpixel(x, y, colour);
			y= y + 1;
			p= p + 2 * dy - 2 * dx;
		}
		else
		{
			ili9341_drawpixel(x, y, colour);
			p = p + 2 * dy;
		}
		x = x + 1;
	}
}

void draw_circle_helper(int xc, int yc, int x, int y, uint16_t colour) 
{
	draw_line(xc - x, yc + y, xc + x, yc + y, colour);
	draw_line(xc - x, yc - y, xc + x, yc - y, colour);
	draw_line(xc - y, yc + x, xc + y, yc + x, colour);
    draw_line(xc - y, yc - x, xc + y, yc - x, colour);
}

void draw_circle(int xc, int yc, int r, uint16_t colour)
{
	int x = 0, y = r;
	int d = 3 - 2 * r;
	draw_circle_helper(xc, yc, x, y, colour);

	while (y >= x) {
		x++;

		if (d > 0) { 
            y--;  
            d = d + 4 * (x - y) + 10; 
        } else
            d = d + 4 * x + 6;

        draw_circle_helper(xc, yc, x, y, colour); 
	}

}

void draw_square(uint16_t x, uint16_t y, uint16_t l, uint16_t colour)
{
	ili9341_fillrect(x - l, y - l, 2 * l, 2 * l, colour);
}

void draw_walls(Wall_Coord walls[], uint8_t walls_no, uint16_t colour)
{
	uint8_t i;
	uint16_t x, y;
	Wall_Coord wall;

	for(i = 0; i < walls_no; i++) {
		wall = walls[i];
		x = wall.top_left_corner.x;
		y = wall.top_left_corner.y;

		/* Quick fix - because the blank space width is not divided
		 * accuratelly, the last wall will not be attached to the margins
		 * of the maze. That's why I'm making its width a lil'bit bigger.
		 */
		if(i + 1 == walls_no)
			wall.wall_width += 4;

		ili9341_fillrect(x, y, wall.wall_width, wall.wall_height, colour);

		if(GAME_MODE) {
			ili9341_drawhline(x, y, wall.wall_width, RED);
			ili9341_drawvline(x, y, wall.wall_height, RED);
			ili9341_drawhline(x, y + wall.wall_height, wall.wall_width, RED);
			ili9341_drawvline(x + wall.wall_width, y, wall.wall_height, RED);
		}
	}
}

void draw_margins(uint16_t colour)
{
	ili9341_fillrect(0, 0, WIDTH, MARGIN_SIZE, colour);
	ili9341_fillrect(0, 0, MARGIN_SIZE, HEIGHT, colour);
	ili9341_fillrect(WIDTH -MARGIN_SIZE, 0, MARGIN_SIZE, HEIGHT, colour);
	ili9341_fillrect(0, HEIGHT - MARGIN_SIZE, WIDTH, MARGIN_SIZE, colour);

	if(GAME_MODE) {
		ili9341_drawhline(MARGIN_SIZE, MARGIN_SIZE, WIDTH - 2 * MARGIN_SIZE, RED);
		ili9341_drawvline(MARGIN_SIZE, MARGIN_SIZE, HEIGHT - 2 * MARGIN_SIZE, RED);
		ili9341_drawhline(MARGIN_SIZE, HEIGHT - MARGIN_SIZE, WIDTH - 2 * MARGIN_SIZE, RED);
		ili9341_drawvline(WIDTH - MARGIN_SIZE, MARGIN_SIZE, HEIGHT - 2 * MARGIN_SIZE, RED);
	}
}

/* SETUP FUNCTIONS */

void LCD_setup()
{
	ili9341_init();
	ili9341_setRotation(1);
}

Circle init_ball(uint16_t radius, uint16_t colour)
{
	Circle ball;
	uint16_t init_dist = 4;

	ball.radius = radius;
	ball.centre.x = MARGIN_SIZE + radius + init_dist;
	ball.centre.y = HEIGHT - MARGIN_SIZE - radius - init_dist;
	ball.colour = colour;

	draw_circle(ball.centre.x, ball.centre.y, radius, colour);

	return ball;
}

Circle get_finish_point(uint16_t radius, uint16_t colour)
{
	Circle finish;
	uint16_t x, y;

	x = WIDTH - MARGIN_SIZE - radius - ERR;
	y = MARGIN_SIZE + radius + ERR;

	finish.centre.x = x;
	finish.centre.y = y;
	finish.radius = radius;
	finish.colour = colour;

	draw_circle(x, y, radius, colour);

	return finish;
}

void generate_fst_walls(Wall_Coord walls[], uint16_t walls_no)
{
	uint16_t blank_width = WIDTH - 2 * MARGIN_SIZE;
	uint16_t blank_height = HEIGHT - 2 * MARGIN_SIZE;	
	uint16_t x;
	uint8_t i = 0;


	/* Multipling by 2 because we want to space the walls
	 * and the distance between them should be the size of the
	 * width of an actual wall. The value should scale with the WIDTH.
	 */
	uint16_t wall_width = blank_width / (walls_no * 2);

	/* The height of a top/bottom wall should scale with the 
	 * HEIGHT. We will settle to 3 quarters of the height.
	 */
	uint16_t wall_height = (blank_height / 4) * 3;

	/* If next wall is 1, then il will be a bottom wall, otherwise
	 * it will be a top wall
	 */
	uint8_t next_wall = 1;


	for(x = wall_width + MARGIN_SIZE; x < WIDTH - MARGIN_SIZE; x += 2 * wall_width) {
		Coord top_left_corner;
		Coord right_bottom_corner;
		Wall_Coord wall_coord;

		if (next_wall) {
			top_left_corner.x = x;
			top_left_corner.y = MARGIN_SIZE + (blank_height / 4);

			right_bottom_corner.x = x + wall_width;
			right_bottom_corner.y = top_left_corner.y + wall_height;

			wall_coord.top_left_corner = top_left_corner;
			wall_coord.right_bottom_corner = right_bottom_corner;
			wall_coord.wall_width = wall_width;
			wall_coord.wall_height = wall_height;

			walls[i++] = wall_coord;
			next_wall = !next_wall;

		} else {
			top_left_corner.x = x;
			top_left_corner.y = MARGIN_SIZE;

			right_bottom_corner.x = x + wall_width;
			right_bottom_corner.y = MARGIN_SIZE + wall_height;

			wall_coord.top_left_corner = top_left_corner;
			wall_coord.right_bottom_corner = right_bottom_corner;
			wall_coord.wall_width = wall_width;
			wall_coord.wall_height = wall_height;

			walls[i++] = wall_coord;
			next_wall = !next_wall;
		}
	}
}

/* GAME PHYSICS FUNCTIONS */

int check_finish(Circle finish, Coord ball, uint16_t radius)
{
	uint16_t start_x = finish.centre.x;
	uint16_t start_y = finish.centre.y - finish.radius;
	uint16_t end_x = finish.centre.x + finish.radius;
	uint16_t end_y = finish.centre.y + finish.radius;

	if(ball.y + radius >= start_y && ball.y - radius <= end_y)
		if(ball.x + radius >= start_x)
			return 1;

	if(ball.x - radius >= start_x && ball.x + radius <= end_x)
		if(ball.y - radius <= finish.centre.y)
			return 1;

	return 0;
}

Coord check_collisions_easy(Coord old_pos, Coord new_pos, uint16_t radius, Wall_Coord walls[], uint16_t walls_no)
{	
	Coord updated_pos = new_pos;
	uint16_t start_x, start_y, end_x, end_y;
	uint16_t ball_x = new_pos.x, ball_y = new_pos.y;
	uint16_t i;

	if (ball_y - radius - ERR <= TOP_MARGIN) {
		updated_pos.y = old_pos.y + ERR;
	}

	if (ball_x - radius - ERR <= LEFT_MARGIN) {
		updated_pos.x = old_pos.x + ERR;
	}

	if (ball_y + radius + ERR >= BOTTOM_MARGIN) {
		updated_pos.y = old_pos.y - ERR;
	}

	if (ball_x + radius + ERR >= RIGHT_MARGIN) {
		updated_pos.x = old_pos.x - ERR;
	}

	for (i = 0; i < walls_no; i++) {

		start_x = walls[i].top_left_corner.x;
		start_y = walls[i].top_left_corner.y;
		end_x = walls[i].right_bottom_corner.x;
		end_y = walls[i].right_bottom_corner.y;

		/* Checking top side of the wall collision */
		if (ball_x + radius + ERR >= start_x && ball_x - radius - ERR <= end_x)
			if (ball_y + radius + ERR >= start_y && ball_y < end_y) {
				updated_pos.y = old_pos.y - ERR;
			}

		/* Checking left side of the wall collision */
		if (ball_y + radius + ERR >= start_y && ball_y - radius - ERR <= end_y)
			if (ball_x + radius + ERR >= start_x && ball_x < end_x) {
				updated_pos.x = old_pos.x - ERR;
			}

		/* Checking bottom side of the wall collision */
		if (ball_x + radius + ERR >= start_x && ball_x - radius - ERR <= end_x)
			if (ball_y - radius - ERR <= end_y && ball_y > start_y) {
				updated_pos.y = old_pos.y + ERR;
			}

		/* Checking right side of the wall collision */
		if (ball_y + radius + ERR >= start_y && ball_y - radius - ERR <= end_y)
			if (ball_x - radius - ERR <= end_x && ball_x > start_x ) {
				updated_pos.x = old_pos.x + ERR;
			}
	}

	return updated_pos;
}

int check_collisions_hard(Coord old_pos, Coord new_pos, uint16_t radius, Wall_Coord walls[], uint16_t walls_no)
{	
	Coord updated_pos = new_pos;
	uint16_t start_x, start_y, end_x, end_y;
	uint16_t ball_x = new_pos.x, ball_y = new_pos.y;
	uint16_t i;

	if (ball_y - radius - ERR <= TOP_MARGIN)
		return 1;

	if (ball_x - radius - ERR <= LEFT_MARGIN)
		return 1;

	if (ball_y + radius + ERR >= BOTTOM_MARGIN)
		return 1;

	if (ball_x + radius + ERR >= RIGHT_MARGIN)
		return 1;

	for (i = 0; i < walls_no; i++) {

		start_x = walls[i].top_left_corner.x;
		start_y = walls[i].top_left_corner.y;
		end_x = walls[i].right_bottom_corner.x;
		end_y = walls[i].right_bottom_corner.y;

		/* Checking top side of the wall collision */
		if (ball_x + radius + ERR >= start_x && ball_x - radius - ERR <= end_x)
			if (ball_y + radius + ERR >= start_y && ball_y < end_y) {
				return 1;
			}

		/* Checking left side of the wall collision */
		if (ball_y + radius + ERR >= start_y && ball_y - radius - ERR <= end_y)
			if (ball_x + radius + ERR >= start_x && ball_x < end_x) {
				return 1;
			}

		/* Checking bottom side of the wall collision */
		if (ball_x + radius + ERR >= start_x && ball_x - radius - ERR <= end_x)
			if (ball_y - radius - ERR <= end_y && ball_y > start_y) {
				return 1;
			}

		/* Checking right side of the wall collision */
		if (ball_y + radius + ERR >= start_y && ball_y - radius - ERR <= end_y)
			if (ball_x - radius - ERR <= end_x && ball_x > start_x ) {
				return 1;
			}
	}

	return 0;

}