#include "game.h"
#include "mpu6050/mpu6050.h"

#define HAVE_FUN_LN 8
static char have_fun[] = {'H', 'A', 'V', 'E', ' ', 'F', 'U', 'N'};

#define HARD_MODE_LN 9
static char hard_mode[] = {'H', 'A', 'R', 'D', ' ', 'M', 'O', 'D', 'E'};

#define WINNER_LN 7
static char winner[] = {'W', 'I', 'N', 'N', 'E', 'R', '!'};

#define LOSER_LN 6
static char loser[] = {'L', 'O', 'S', 'E', 'R', '!'};

void print_msg(char msg[], uint16_t msg_len)
{
	ili9341_clear(BLACK);
	_delay_ms(1000);

	ili9341_setcursor(100, 50);
	ili9341_settextcolour(RED, BLACK);
	ili9341_settextsize(5);

	for (int i = 0; i < msg_len; i++)
		ili9341_write(msg[i]);
	_delay_ms(30000);

	ili9341_clear(BLACK);
	_delay_ms(1000);
}

int main(void)
{	
	LCD_setup();
	
	/* MPU6050 Accelerometer Setup */
	sei();
	mpu6050_init();
	_delay_ms(50);

	double axg = 0, ayg = 0, azg = 0;
    double gxds = 0, gyds = 0, gzds = 0;

	/* It's all about the fun! */
	GAME_MODE = EASY;
	print_msg(have_fun, HAVE_FUN_LN);

    /* Maze Setup */
	draw_margins(CYAN);

	uint16_t walls_no = 3;
	Wall_Coord walls[walls_no];

	generate_fst_walls(walls, walls_no);
	draw_walls(walls, walls_no, CYAN);

	/* Ball Setup */
	uint16_t radius = 7;
	Circle ball = init_ball(radius, RED);	
	Coord new_centre, old_centre;

	/* Finish Point */
	Circle finish_point = get_finish_point(radius + 1, YELLOW);

	while (1) {

		while (1) {

			if(check_finish(finish_point, ball.centre, radius)){
				GAME_STATE = WON;
				break;
			}

			mpu6050_getConvData(&axg, &ayg, &azg, &gxds, &gyds, &gzds);
			_delay_ms(50);

			uint16_t move_x = (uint16_t)(-10 * axg);
			uint16_t move_y = (uint16_t)(-7 * ayg);

			new_centre.x = ball.centre.x + move_x;
			new_centre.y = ball.centre.y + move_y;

			old_centre.x = ball.centre.x;
			old_centre.y = ball.centre.y;

			if(GAME_MODE == EASY)
				ball.centre = check_collisions_easy(ball.centre, new_centre, 
							  ball.radius, walls, walls_no);

			else if (check_collisions_hard(ball.centre, new_centre, 
							  ball.radius, walls, walls_no)) 
			{
				GAME_STATE = LOST;
				break;
			} else
				ball.centre = new_centre;

			draw_square(old_centre.x, old_centre.y, ball.radius + 1, BLACK);
			draw_circle(ball.centre.x, ball.centre.y, ball.radius, RED);

		}

		if(GAME_MODE == EASY) {
			print_msg(hard_mode, HARD_MODE_LN);
			GAME_MODE = HARD;

			draw_margins(CYAN);
			draw_walls(walls, walls_no, CYAN);
			ball = init_ball(radius, RED);
			finish_point = get_finish_point(radius + 1, YELLOW);

		} else if (GAME_MODE == HARD) {

			if(GAME_STATE == WON)
				print_msg(winner, WINNER_LN);
			else 
				print_msg(loser, LOSER_LN);

			break;
		}
	}

}