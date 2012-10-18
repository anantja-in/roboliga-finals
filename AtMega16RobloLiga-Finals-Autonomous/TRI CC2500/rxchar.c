#include <avr/io.h>
#include <util/delay.h>
#include<avr/interrupt.h>

#include "uart.h"

#define BIT(x)	(1 << (x))
#define CHECKBIT(x,b) (x&b)		//Checks bit status
#define SETBIT(x,b) x|=b;		//Sets the particular bit
#define CLEARBIT(x,b) x&=~b;	//Sets the particular bit
#define TOGGLEBIT(x,b) x^=b;	//Toggles the particular bit

unsigned char temp;

void delay_ms(unsigned int t) { // deprecated
	unsigned int i,j;
	for (j=0;j<t;j++) {
		for(i=0;i<6500;i++) {}
	}
}


/*ISR(USART_RXC_vect) // UART receiver interrupt service routine
{
	temp = UDR;
	if(temp=='W')
	{
		trans('X');
	}
	else if(temp=='Y') {
		_delay_ms(2000);
		trans('Z');
	}
	else {
		trans(temp);
	}
}*/

int main(void) {
	uart_init();
	//sei();

	//RF module initialisation
	trans('1');	// setting self adress as 1
	trans('2');	//  setting remote adress  2
	trans('3');	// selecting channel as 3 
	
	while(1) 
	{
		trans(uart_rx_char());
	}


	return 0;
}
