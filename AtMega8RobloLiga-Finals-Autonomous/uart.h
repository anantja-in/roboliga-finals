
void uart_init (void) {
	UCSRA = 0x00;
	UCSRB = 0x98;
	UCSRC = 0x86;
	UBRRH=0;
	UBRRL = 25; //38400 bps
}


void trans(unsigned char data) {
	UDR = data;
	while(!(UCSRA&(1<<UDRE))); //wait till Data buffer is not empty
	UCSRA |= 0x40;
	_delay_ms(1);
}



void uart_tx_string(char string[15]) {
		int a=0;
		while(string[a] !='\0') {
			trans(string[a]);
			a+=1;
		}
}
	
// Deprecated function	
unsigned char uart_rx_char (void) {
	while(!(UCSRA & (1<<RXC)));
	UCSRA |= 0x80;
	return UDR;
}