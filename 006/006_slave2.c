#include "..\LLL\ASA_Lib.h"
#include "..\LLL\ASA_USART.h"

volatile int temp;
volatile int count;
volatile int count1;
volatile int count2;

int main(void){
	ASA_M128_set();
	uart_slave_ini();
	UARTS_tb_Ini();
	M128_ASAID_SET(1);
	sei();
	UARTS_str_Ini();
	temp=0;
	count=0;
	printf("---------slave----------test---------------\n");
	while(1){
		UARTS_dppk();
		// if (UARTS_trm_buffer.BRi!=UARTS_trm_buffer.BWi) {
		// 	printf("txccc  %d\n", count);
		// 	printf("bmR %d\n", UARTS_trm_buffer.BRi);
		// 	printf("bmw %d\n", UARTS_trm_buffer.BWi);
		// 	for (int i = 0; i < (MaxPackBytes+4); ++i) {
		// 		printf("%d   ", UARTS_trm_buffer.Buffer[i]);
		// 	}
		// 	printf("\n");
		// }
	}
}

ISR(USART1_RX_vect){
	M128_UARTS_rx();
}
ISR(USART1_TX_vect){
	M128_UARTS_tx();
}
