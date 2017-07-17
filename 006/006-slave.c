#include "..\LLL\ASA_Lib.h"
#include "..\LLL\ASA_USART.h"

volatile int temp;
volatile int count;
volatile int count1;
volatile int count2;

int main(void){
	ASA_M128_set();
	M128_ASAID_SET(1);
	uart_ini();
	sei();
	temp=0;
	count=0;
	printf("---------slave----------test---------------\n");
	while(1){
		UARTS_dppk();
		for (int i = 0; i < MaxPackBytes; ++i) {
			printf("%d  ", UARTS_rec_str.Buffer[i]);
		}
		printf("\n");
		_delay_ms(500);
	}
}

ISR(USART1_RX_vect){
	if( UCSR1B&(1<<RXB81) ){
		if ( UDR1==USART_ID ){
			UCSR1A&= ~(1<<MPCM1);
			//MPCM disable, data frames receiving
		}
		else{
			UCSR1A|=(1<<MPCM1);
			//MPCM enable, adderss frames receiving
		}
	}
	else{
		UARTS_rec_str.Buffer[(int)UARTS_rec_str.BWi]=UDR1;
		UARTS_rec_str.BWi=(UARTS_rec_str.BWi+1)%MaxPackBytes;
	}
	// M128_UARTS_rx();
}
