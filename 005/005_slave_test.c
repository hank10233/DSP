#include "..\LLL\TWI_slave.h"
#include "..\LLL\ASA_Lib.h"

int main(void){
	ASA_M128_set();
	slave_TWI_ini();
	slave_TWI_swap_ini(&slave_TWI_swap_str,&Res_Buff_str,&Com_Buff_str);
	slave_TWI_PacDe_ini(&slave_TWI_PacDe_str,&Res_Buff_str,&Com_Buff_str);
		printf("-----------start---------------\n");
	while(1){
		slave_TWI_PacDe_step(&slave_TWI_PacDe_str);
	}
}

ISR(TWI_vect){
	slave_TWI_swap_step();
}
