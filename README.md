# Microcontroller-Ball-in-a-Maze

Using an ATMega 324A MCU, an accelerometer and 2.8" LCD, I implemented a puzzle meant to test the player's dexterity and
hand-eye coordination: Ball in a Maze.

The game runs as a bare-metal application.

The accelerometer constantly measures the inclination angles of the device on the X-Y axes and sends these measurements
to the MCU which processes the data and renders the position of the ball in the maze onto the LCD.
