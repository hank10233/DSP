#include "..\LLL\TWI_slave.h"
#include "..\LLL\ASA_Lib.h"

int main(void){
	ASA_M128_set();
	slave_TWI_ini();
	// slave_TWI_swap_ini(&slave_TWI_swap_str,&Res_Buff_str,&Com_Buff_str);
	// slave_TWI_PacDe_ini(&slave_TWI_PacDe_str,&Res_Buff_str,&Com_Buff_str);
	// 	printf("-----------start---------------\n");
	// while(1){
	// 	slave_TWI_PacDe_step(&slave_TWI_PacDe_str);
	// }

	int a;
	printf("start-----\n");
	while(1){
		if(TW_STATUS!=0xf8)printf("%x\n",TW_STATUS );
		switch(TW_STATUS){
			case TWI_SR_SLA_ACK:
				TWCR=(1<<TWINT)|(1<<TWEA)|(1<<TWEN)|(1<<TWIE);
				break;
			case TWI_SR_DATA_ACK:
				a=TWDR;
				printf("%d\n", a);
				TWCR=(1<<TWINT)|(1<<TWEA)|(1<<TWEN)|(1<<TWIE);
				break;
			case TWI_SR_STOP:
				TWCR=(1<<TWINT)|(1<<TWEA)|(1<<TWEN)|(1<<TWIE);
				break;
		}
	}

}

ISR(TWI_vect){
	// slave_TWI_swap_step();
}
