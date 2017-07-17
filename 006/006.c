#include "..\LLL\ASA_Lib.h"
#include "..\LLL\ASA_USART.h"

volatile char a[35];
volatile int f;

int main(void){
	ASA_M128_set();
	uart_master_ini();
	UARTS_mb_Ini();
	M128_ASAID_SET(1);
	char check;
	char bg[70];
	char cg[70]={0};
	for (int i = 0; i < 70; ++i)
	{
		bg[i]=i;
	}
	sei();
	printf("---------test----------test---------------\n");
	check=M128_UARTM_tpk(3, USART_ID, 3, 70, bg);
	printf("1 %d\n", check);
	_delay_ms(500);
	check=M128_UARTM_tpk(3, USART_ID, 3, 0, bg);
	printf("2 %d\n", check);
	_delay_ms(500);
	check=M128_UARTM_tpk(3, USART_ID, 3, 6, bg);
	printf("3 %d\n", check);
	_delay_ms(200);
	check=M128_UARTM_rpk(3, USART_ID, 3, 70, cg);
	printf("1 %d\n", check);
	_delay_ms(500);
	check=M128_UARTM_rpk(3, USART_ID, 3, 0, cg);
	printf("2 %d\n", check);
	_delay_ms(500);
	check=M128_UARTM_rpk(3, USART_ID, 3, 6, cg);
	printf("3 %d\n---------------rpk\n", check);

	for(int j=0;j<70;j++) {
		printf("%d   ",cg[j]);
	}


	while(1){



		_delay_ms(500);
	}
}

ISR(USART1_RX_vect){
	M128_UARTS_rx2();
}
