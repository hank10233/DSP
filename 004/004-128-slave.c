#include "..\LLL\ASA_Lib.h"
#include "SPI_slave.h"

int main(void){
	ASA_M128_set();
	slave_ini();
	sei();
	slave_SPI_swap_ini(&slave_SPI_swap_str,&Res_Buff_str,&Com_Buff_str);
	slave_SPI_PacDe_ini(&slave_SPI_PacDe_str,&Res_Buff_str,&Com_Buff_str);

	while(1){

		slave_SPI_PacDe_step(&slave_SPI_PacDe_str);
		/*
		for(int i=0;i<MAXBUFFBYTES;i++){
			printf("%d ",str_p->InBUFF_p->data[ i ]);
		}
		printf("\n--------------------\n");


		for(int i=0;i<MAXBUFFBYTES;i++){
			printf("%d ",str_p->OutBUFF_p->data[ i ]);
		}
		printf("\n--------------------\n");
		 */
	}

}

ISR(SPI_STC_vect){
	slave_SPI_swap_step();
}

