#include <avr/io.h>
#include <util/delay.h>
#include<avr/interrupt.h>
#include "uart.h"

/*Macros definition*/
#define BIT(x)	(1 << (x))			//Set a particular bit mask
#define CHECKBIT(x,b) x&b			//Checks bit status
#define SETBIT(x,b) x|=b;			//Sets the particular bit
#define CLEARBIT(x,b) x&=~b;		//Sets the particular bit
#define TOGGLEBIT(x,b) x^=b;		//Toggles the particular bit

#define East 0
#define North 1
#define West 2
#define South 3

#define WHITE 1
#define BLACK 0
#define True 1
#define False 0

#define led2off PORTD|=0b01000000;
#define led2on PORTD&=0b10111111;

#define led1off PORTD|=0b10000000;
#define led1on PORTD&=0b01111111;

#define pulse 75
#define sp 5

//Global variables:
unsigned char X;
unsigned char Y;
unsigned char curHead;
unsigned char globalCoord;


// For digital Output:
int s1() { if(PINA&0x01) return 1; else return 0; } //leftmost, viewed from top;
int s2() { if(PINA&0x02) return 1; else return 0; }
int s3() { if(PINA&0x04) return 1; else return 0; }
int s4() { if(PINA&0x08) return 1; else return 0; }
int s5() { if(PINA&0x10) return 1; else return 0; }



int mod(int a) {
	if(a<0)
			return a*-1;
	return a;
}

void delay_ms(int x) {
	for(int i=0;i!=x;i++) {
			while(!(TIFR & (1<<TOV0)));
			TCNT0 = 0x63;
			TIFR |= (1<<TOV0); /* Timer restarts as soon as you configure this. 
			Hence TCNT0 must be written before TIFR */	
		}

}
void speed(int j, int dir) {
	if(j>5) j=5;
		PORTB |= dir;
		for(int i=0;i!=j;i++) {
			while(!(TIFR & (1<<TOV0)));
			TCNT0 = 0x63;
			TIFR |= (1<<TOV0); /* Timer restarts as soon as you configure this. 
			Hence TCNT0 must be written before TIFR */
		}
		PORTB &= 0xF0;
		for(int k=0;k!=5-j;k++) {
			while(!(TIFR & (1<<TOV0)));
			TCNT0 = 0x63;
			TIFR |= (1<<TOV0); 
		}

}


void fwd() {
	speed(sp,0x0A);
}

void left() {
	speed(sp,0x06);
}
void leftNonDiff() {
	speed(sp,0x02);
}
void right() {
	speed(sp,0x09);
}
void rightNonDiff() {
	speed(sp,0x08);
}
void back() {
	speed(sp, 0x05);
}

void stop() {
	PORTB = 0x00;
}

void mvfwdpulse() {
	int x;
	PORTB = 0x0A;
	for(x=0;x!=pulse;x++) {
		delay_ms(1);
	}
	stop();
}
void mvbwdpulse() {
	int x;
	PORTB = 0x05;
	for(x=0;x!=pulse;x++) {
		delay_ms(1);
	}
	stop();
} 


//--------------------------------------------------------------------//
void turnRight() {
	// Take a right turn !!
	int t=0,s=0;
	//mvfwdpulse();
	while((s1()==BLACK) | (s5()==BLACK))
		fwd();
	
	while((t==0 | s==0) | s3()==WHITE) {
		right();
		if(s1()==BLACK) t=1;
		if(s5()==BLACK) s=1;
	}
	fwd();
	curHead+=3;
	curHead%=4;
	// Right turn complete
}

void turnLeft() {
	// Take a left turn !!
	int t=0, s=0;
	//mvfwdpulse();
	while((s1()==BLACK) | (s5()==BLACK))
		fwd();

	while(t==0 |s==0 | s3()==WHITE) {
		left();
		if(s5()==BLACK) t=1;
		if(s1()==BLACK) s=1;
	}
	fwd();
	curHead+=1;
	curHead%=4;
	// Left turn complete
}

/*
void aboutTurn() {
	// Take an about turn from left!!
	int t=0;
	int s=0;

	mvfwdpulse();
	while(t==0 |s==0 | s3()==WHITE) {
		left();
		if(s5()==BLACK) t=1;
		if(s1()==BLACK) s=1;
	}
	t=0; s=0;
		while(t==0 |s==0 | s3()==WHITE) {
		left();
		if(s5()==BLACK) t=1;
		if(s1()==BLACK) s=1;
	}

	fwd();
	curHead+=2;
	curHead%=4;
	// About turn complete
}

*/

void moveStraight() {
	// Makes the bot follow the line till it finds the next intersection and then returns
	//mvfwdpulse();
	while((s1()==BLACK) | (s5()==BLACK))
		fwd();
	while(!((s1()==BLACK) | (s5()==BLACK))) {
		fwd();
		if(s3()==WHITE) {
			if(s2()==BLACK) {
				while(s3()!=BLACK)
					leftNonDiff();
			}
			else if(s4()==BLACK) { 
				while(s3()!=BLACK)
					rightNonDiff();
			}
		}
	}
}
void init_motor(void)
{
	DDRB = 0xFF; // 1 for output
	PORTB = 0x00; // 1 for input
	// Shall use PORTB motors only
	// B1--B4 are the motors
}
void init_sensor(void) 
{
	DDRC = 0x00; //input
	PORTC = 0xFF;
	
	DDRD = 0xFF;
	PORTD = 0x00;
	PORTD |= 0xFF;
}

void move(unsigned char coord) {

	int distX;
	int distY;
	int x,y,z;
	unsigned char temp;
	
	// Calc distX and distY to coord now;
	temp = coord;
	temp &= 0xF0;
	temp = temp/16;
	distX = temp-X;
	X=temp;
	
	temp = coord;
	temp &= 0x0F;
	distY = temp-Y;
	Y= temp;
	
	
	// First move X
	if (distX > 0) {
		temp = curHead;
		temp %=4; // Remove this, not required
		if(temp<=2) {
			for(x=0;x<temp;x++)
			turnRight();
			curHead+=3;
			curHead%=4;
		}
		else { // for head = South
			turnLeft();
			curHead+=1;
			curHead%=4;
		}
	}
	else { // distX < 0
		temp = curHead+2;
		temp %=4;
		if(temp<=2) {
			for(x=0;x<temp;x++)
			turnRight();
			curHead+=3;
			curHead%=4;
		}
		else {
			turnLeft();
			curHead+=1;
			curHead%=4;
		}
	}
	
	for(x=0;x>mod(distX);x++)
		moveStraight();
	
	// Now move Y
	if (distY > 0) {
		temp = curHead-1;
		temp %=4; // Remove this, not required
		if(temp<=2) {
			for(x=0;x<temp;x++)
			turnRight();
			curHead+=3;
			curHead%=4;
		}
		else {
			turnLeft();
			curHead+=1;
			curHead%=4;
		}
	}
	else { // distY < 0
		temp = curHead-3;
		temp %=4;
		if(temp<=2) {
			for(x=0;x<temp;x++)
			turnRight();
			curHead+=3;
			curHead%=4;
		}
		else {
			turnLeft();
			curHead+=1;
			curHead%=4;
		}
	}
	
	for(x=0;x>mod(distY);x++)
		moveStraight();
		
	trans('Z');
	
}


//RX complete Interrup routine..put here all the code that you want to do with the received data..
ISR(USART_RXC_vect)
{
	unsigned char Recv=UDR;
	if(Recv=='W')
		trans('X');
	else if(Recv=='Y')
		move(globalCoord);
	else {
		globalCoord=Recv;
		trans(globalCoord);
	}
	
}

int main (void)
{
	//Initialize UART with 8-bit character sizes,no parity,1 stop bit 
	uart_init();
	sei();
	
	init_motor();	//This should be called first before using any motor command macro
	init_sensor();
	
	TCNT0 = 0x63; // for speed control
	TCCR0 = 0x05;

	X=1;
	Y=1;
	curHead = East;

	while(1) {
		
		trans('1');
		_delay_ms(1);
		trans('2');
		_delay_ms(1);
		trans('3');
		_delay_ms(1);
		
		
			
			fwd();
		//move(0x22);
		//moveStraight();
		//turnRight();
		//stop();
		//while(1);
		//moveStraight();
		
		// Now ISR will initiate the actions
	
		
	}
}
