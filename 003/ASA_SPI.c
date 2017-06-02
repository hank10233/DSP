#include "ASA_SPI.h"

char M128_SPI_swap(char Data){
	SPDR = Data;
	while( !(bit_check(SPSR,SPIF)) );
	return SPDR;
}


char M128_SPI_set(char LSByte, char Mask,  char shift,  char Data) {
	if(LSByte < 200 || LSByte > 201) { return 1; }
	if(shift  < 0   || shift  >   7) { return 1; }

	//set spi pins on port b MOSI SCK SS output, MISO input
	char MSTR_DDR=0x00;
	bit_set(MSTR_DDR,DD_MOSI);
	bit_set(MSTR_DDR,DD_SCK);
	bit_set(MSTR_DDR,DD_SS);
	M128_DIO_set(200+DDR_SPI_num,0x0f,0, 0);
	M128_DIO_set(200+DDR_SPI_num,0x0f,0, MSTR_DDR);

	//set CS pin (PF4) output the setting of CPOL
	M128_DIO_set(200+CS_PORT_NUM, CS_PORT_MSK, CS_PORT_SHT, 1);

	if ( LSByte==200 ) {
		bits_put(Data, SPCR, Mask, shift);
	}
	else if ( LSByte==201 ) {
		bits_put(Data, SPSR, Mask, shift);
	}
	M128_DIO_fpt(CS_PORT_NUM, CS_PORT_MSK, CS_PORT_SHT,(char)bit_get(SPCR,CPOL));
	return 0;
}

char M128_SPI_put(char NoAdd, char Addr, char Bytes, void *Data_p) {
	if ( NoAdd > 1 ) { return 1; }
	if ( Bytes < 0 ) { return 1; }

	M128_DIO_fpt(CS_PORT_NUM, CS_PORT_MSK, CS_PORT_SHT,!((char)bit_get(SPCR,CPOL)));

	if ( NoAdd==0 ) {
		M128_SPI_swap(Addr);
		if ( bit_check(SPSR,WCOL) ) { return 1; }
	}
	for (int i = 0; i < Bytes; i++) {
		*((char*)Data_p +i) = M128_SPI_swap( *((char*)Data_p +i) );
		if ( bit_check(SPSR,WCOL) ) { return 1; }
	}

	M128_DIO_fpt(CS_PORT_NUM, CS_PORT_MSK, CS_PORT_SHT,(char)bit_get(SPCR,CPOL));

	return 0;
}

char M128_SPI_get(char NoAdd, char Addr, char Bytes, void *Data_p) {
	if ( NoAdd > 1 ) { return 1; }
	if ( Bytes < 0 ) { return 1; }

	M128_DIO_fpt(CS_PORT_NUM, CS_PORT_MSK, CS_PORT_SHT,!((char)bit_get(SPCR,CPOL)));

	if ( NoAdd==0 ) {
		M128_SPI_swap(Addr);
		if ( bit_check(SPSR,WCOL) ) { return 1; }
	}
	for (int i = 0; i < Bytes; i++) {
		*((char*)Data_p +i) = M128_SPI_swap( *((char*)Data_p +i) );
		if ( bit_check(SPSR,WCOL) ) { return 1; }
	}

	M128_DIO_fpt(CS_PORT_NUM, CS_PORT_MSK, CS_PORT_SHT,0);

	return 0;
}
