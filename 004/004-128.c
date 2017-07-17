
#include "..\LLL\ASA_Lib.h"
#include "..\LLL\ASA_SPI.h"

#define HEADER 0x87
#define REHEADER 0x88
#define ASA_ID 1
#define CALL_TYPE_SET 0
#define CALL_TYPE_PUT 1
#define CALL_TYPE_GET 2
#define CALL_TYPE_FPT 3
#define CALL_TYPE_FGT 4


void ASA_master_ini(){
    // set ADDR pins (PB5~7) as output
    DDRB=(1<<PB5)|(1<<PB6)|(1<<PB7)|(1<<DD_SCK)|(1<<DD_MOSI);
    //set CS as output
    DDRF=(1<<PF4);
	SPCR=(1<<SPE)|(1<<MSTR);
	//pull up SS
	PORTB|=(1<<DD_SS);
}

int main(void){
	ASA_M128_set();
	ASA_master_ini();
	char f[2],g[2];
	f[0]=3;
	f[1]=5;
	g[0]=12;
	g[1]=45;
	char check;
	check=ASA_SPI_set(ASA_ID,200,0xff,0,5);
	printf("set  %d\n",check);
	//_delay_ms(1000);

	check=ASA_SPI_put(ASA_ID,0,2,f);
	printf("put  %d\n",check);
	//_delay_ms(1000);

	check=ASA_SPI_put(ASA_ID,0,2,g);
	printf("put  %d\n",check);
	//_delay_ms(1000);

	check=ASA_SPI_get(ASA_ID,100,2,f);
	printf("get  %d\n",check);
	printf("f %d  %d\n",f[0],f[1]);
	//_delay_ms(1000);
	check=ASA_SPI_fpt(ASA_ID,200,0xff,1,1);
	printf("fpt  %d\n",check);
	//_delay_ms(1000);
	check=ASA_SPI_fgt(ASA_ID,100,0xff,1,f);
	printf("fgt  %d\n",check);
	printf("f %d  %d\n",f[0],f[1]);
}
