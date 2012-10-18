#include<avr/io.h>
#define led2off PORTB&=0xBF;
#define led2on PORTB|=0x40;

#define led1on PORTC|=0x02;
#define led1off PORTC&=0xFD;

int sp = 4;

void delay_ms(int x) {
	for(int i=0;i!=x;i++) {
			while(!(TIFR & (1<<TOV0)));
			TCNT0 = 0x63;
			TIFR |= (1<<TOV0); /* Timer restarts as soon as you configure this. 
			Hence TCNT0 must be written before TIFR */	
		}

}

void ledToggle() {
	led2on
	delay_ms(50);
	led2off
	delay_ms(50);
}


void speed(int j, int dir) {
	if(j>5) j=5;
		PORTB = dir;
		for(int i=0;i!=j;i++) {
			while(!(TIFR & (1<<TOV0)));
			TCNT0 = 0x63;
			TIFR |= (1<<TOV0); /* Timer restarts as soon as you configure this. 
			Hence TCNT0 must be written before TIFR */
		}
		PORTB = 0x00;
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
	speed(3,0x09);
}

void right() {
	speed(3, 0x06 );
}

void back() {
	speed(sp, 0x05);
}

void stop() {
	PORTB = 0x00;
}

int main(void) {

	TCNT0 = 0x63;
	TCCR0 = 0x05;

	DDRA = 0x00;
	PORTA = 0xFF;
	
	DDRB = 0b11101111; // 1 for output
	PORTB = 0b00010000; // 1 for input

	DDRD = 0b11110011;
	PORTD = 0b00001100;

	sp=1;
	while((PINB & 0x10)) { // Sw1
		if( PIND & 0x04 )
			sp+=1;

		led2on
		delay_ms(10*sp);
		led2off
		delay_ms(10*sp);
	
	}
		
	while(1) {
		if((PINA & 0x01)) {
			fwd();
		}
		else if((PINA & 0x02)) {
			back();
		}
		else if((PINA & 0x04)) {
			left();
		}
		else if((PINA & 0x08)) {
			right();
		}
		else {
			stop();
		}
	}
}

