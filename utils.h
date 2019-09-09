#include<avr/io.h>
#include<util/delay.h>
#include<stdio.h>
#include<stdbool.h>
#include<inttypes.h>

#define WIDTH 320
#define HEIGHT 240
#define MARGIN_SIZE 8

#define TOP_MARGIN MARGIN_SIZE // Collision on y < TOP_MARGIN
#define LEFT_MARGIN MARGIN_SIZE // Collision on x < LEFT_MARGIN
#define BOTTOM_MARGIN HEIGHT - MARGIN_SIZE // Collision on y > BOTTOM_MARGIN
#define RIGHT_MARGIN WIDTH - MARGIN_SIZE // Collision on x > RIGHT_MARGIN

#define ERR 1 //Used for collision detection

int GAME_MODE;
#define EASY 0
#define HARD 1

int GAME_STATE;
#define WON 1
#define LOST 0

typedef struct {
	uint16_t x;
	uint16_t y;
}Coord;

typedef struct{
	Coord top_left_corner;
	Coord right_bottom_corner;
	uint16_t wall_width;
	uint16_t wall_height;
}Wall_Coord;

typedef struct {
	Coord centre;
	uint16_t radius;
	uint16_t colour;
}Circle;