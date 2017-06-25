#include "..\LLL\ASA_TWI.h"
#include "..\LLL\ASA_Lib.h"

#define twi_slave_address 0b10110000
#define HEADER 0x77
#define REHEADER 0x79
#define ASA_ID 1
#define CALL_TYPE_SET 0
#define CALL_TYPE_PUT 1
#define CALL_TYPE_GET 2
#define CALL_TYPE_FPT 3
#define CALL_TYPE_FGT 4

int main(void){
	ASA_M128_set();
	M128_TWI_ini();
	M128_ASAID_SET(ASA_ID);
	char check;
	char pdata[2];
	char gdata[2];
	pdata[0]=56;
	pdata[1]=77;
	check=ASA_TWI_set(ASA_ID, twi_slave_address, 200, 0xff,0, 0x0f);
	printf("set check  %d\n",check );
	check=ASA_TWI_put(ASA_ID, twi_slave_address, 0, 2, &pdata);
	printf("put check  %d\n",check );
	check=ASA_TWI_get(ASA_ID, twi_slave_address, 100, 2, gdata);
	printf("get check  %d\n",check );
	printf("g1  %d\n", gdata[0]);
	printf("g2  %d\n", gdata[1]);
	check=ASA_TWI_fpt(ASA_ID, twi_slave_address, 200, 0xff,6,1);
	printf("fpt check  %d\n",check );
	check=ASA_TWI_get(ASA_ID, twi_slave_address, 100, 2, gdata);
	printf("get check  %d\n",check );
	printf("g1  %d\n", gdata[0]);
	printf("g2  %d\n", gdata[1]);
	check=ASA_TWI_fgt(ASA_ID, twi_slave_address, 100, 0xff,4,gdata);
	printf("fpt check  %d\n",check );
	printf("g1  %d\n", gdata[0]);
	printf("g2  %d\n", gdata[1]);
	while(1);
	return 0;
}
