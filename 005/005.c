#include "..\LLL\ASA_Lib.h"
#include "..\LLL\ASA_DIO.h"
#include "..\LLL\ASA_TWI.h"

#define ASA_ID 1
//Internal Definitions
#define THM01_Put_TmpReg			0x00		//Pointer Register for Temperature Register of Tmp175
#define THM01_Set_ConfgReg			0x01		//Configuration Register for Temperature Register of Tmp175
#define THM01_Set_Format			0x60

int main(){
	ASA_M128_set();
	M128_ASAID_SET(ASA_ID);
	M128_TWI_set(200,0xff,0,0xc8);
	M128_TWI_set(202, (1<<TWEN), TWEN, 1);
	char momoda[2]={0};
	float tmpdata;
	char check;
	unsigned int SLA_ADD;
	SLA_ADD = 0b10110000;
	char data;
	data=THM01_Set_Format;
	printf("------------------------------------------\n");
	check=M128_TWI_trm(2, SLA_ADD, THM01_Set_ConfgReg, 1 ,&data);
	printf("%d\n",check );
	_delay_ms(1000);
	check=M128_TWI_rec(2 , SLA_ADD , THM01_Put_TmpReg , 1 , &momoda);
	printf("%d\n",check );
	printf("%d\n",momoda[0] );
	printf("%d\n",momoda[1] );
	//tmpdata = ((momoda[0]<<8)+momoda[1])>>4;
	//tmpdata = tmpdata*0.0625;
	//printf("main tmp  %d\n", (int)tmpdata);

	while(1){}
	return 0;
}

