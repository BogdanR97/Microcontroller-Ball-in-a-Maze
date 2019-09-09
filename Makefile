all: main.hex

main.hex: main.elf 
	avr-objcopy  -j .text -j .data -O ihex  main.elf main.hex
	avr-size main.elf

main.elf: main.o ili9341gfx.o ili9341_0.o mpu6050.o mpu6050dmp6.o twimastertimeout.o game.o
	avr-gcc -mmcu=atmega324a  -Wall -fdata-sections -ffunction-sections -o $@ $^ -Wl,--gc-sections

main.o: main.c
	avr-gcc -mmcu=atmega324a  -Wall -O1 -fdata-sections -ffunction-sections -c main.c

game.o: game.c
	avr-gcc -mmcu=atmega324a  -Wall -O1 -fdata-sections -ffunction-sections -c game.c

ili9341gfx.o: ili9341gfx.c
	avr-gcc -mmcu=atmega324a -Wall -O1 -fdata-sections -ffunction-sections -c ili9341gfx.c
	
ili9341_0.o: ili9341_0.c
	avr-gcc -mmcu=atmega324a -Wall  -O1 -c ili9341_0.c
	
mpu6050.o : mpu6050/mpu6050.c
	avr-gcc -mmcu=atmega324a -DF_CPU=16000000 -Wall  -O1 -c mpu6050/mpu6050.c

mpu6050dmp6.o: mpu6050/mpu6050dmp6.c
	avr-gcc -mmcu=atmega324a  -Wall  -O1 -c mpu6050/mpu6050dmp6.c

twimastertimeout.o: i2chw/twimastertimeout.c
	avr-gcc -mmcu=atmega324a -Wall  -O1 -c i2chw/twimastertimeout.c



clean:
	rm -rf main.hex main.elf main.o ili9341gfx.o ili9341_0.o mpu6050.o mpu6050dmp6.o twimastertimeout.o game.o
