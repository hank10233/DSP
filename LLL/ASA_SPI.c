#include "ASA_SPI.h"
#include <stdio.h>

void Master_ini(){
	//set spi pins on port b MOSI SCK SS output, MISO input
	char MSTR_DDR=0x00;
	bit_set(MSTR_DDR,DD_MOSI);
	bit_set(MSTR_DDR,DD_SCK);
	bit_set(MSTR_DDR,DD_SS);
	M128_DIO_set(200+DDR_SPI_num,0x0f,0, 0);
	M128_DIO_set(200+DDR_SPI_num,0x0f,0, MSTR_DDR);
	// set ADDR pins (PB5~7) as output
	// M128_DIO_set(200+ADDR_PORT_num,ADDR_PORT_msk,ADDR_PORT_sht,7);
	//set CS pin (PF4) output 1
	M128_DIO_set(200+CS_PORT_NUM, CS_PORT_MSK, CS_PORT_SHT,1);
	// SPCR=(1<<SPE)|(1<<MSTR);
	M128_SPI_set(200, (1<<SPE)|(1<<MSTR) , 0 , 0xff);
}

char M128_SPI_swap(char Data){
	M128_DIO_fpt(CS_PORT_NUM, CS_PORT_MSK, CS_PORT_SHT,1);
	SPDR = Data;
	while( !(bit_check(SPSR,SPIF)) );
	M128_DIO_fpt(CS_PORT_NUM, CS_PORT_MSK, CS_PORT_SHT,0);
	return SPDR;
}


char M128_SPI_set(char LSByte, char Mask,  char shift,  char Data) {
	if(LSByte < 200 || LSByte > 201) { return 1; }
	if(shift  < 0   || shift  >   7) { return 1; }

	if ( LSByte==200 ) {
		bits_put(Data, SPCR, Mask, shift);
	}
	else if ( LSByte==201 ) {
		bits_put(Data, SPSR, Mask, shift);
	}
	M128_DIO_fpt(CS_PORT_NUM, CS_PORT_MSK, CS_PORT_SHT,0);
	return 0;
}

char M128_SPI_put(char NoAdd, char Addr, char Bytes, void *Data_p) {
	if ( NoAdd > 1 ) { return 1; }
	if ( Bytes < 0 ) { return 1; }

	if ( NoAdd==0 ) {
		M128_SPI_swap(Addr);
		if ( bit_check(SPSR,WCOL) ) { return 1; }
	}
	for (int i = 0; i < Bytes; i++) {
		*((char*)Data_p +i) = M128_SPI_swap( *((char*)Data_p +i) );
		if ( bit_check(SPSR,WCOL) ) { return 1; }
	}
	return 0;
}

char M128_SPI_get(char NoAdd, char Addr, char Bytes, void *Data_p) {
	if ( NoAdd > 1 ) { return 1; }
	if ( Bytes < 0 ) { return 1; }

	if ( NoAdd==0 ) {
		M128_SPI_swap(Addr);
		if ( bit_check(SPSR,WCOL) ) { return 1; }
	}
	for (int i = 0; i < Bytes; i++) {
		*((char*)Data_p +i) = M128_SPI_swap( *((char*)Data_p +i) );
		if ( bit_check(SPSR,WCOL) ) { return 1; }
	}
	return 0;
}

char ASA_SPI_set(char ASAID,char LSBbyte,char Mask,char Shift,char Data){
	char check_sum=0;
	char putdata=0;
	char* putdata_p=&putdata;
	char getdata=0;
	char* getdata_p=&getdata;
	char result;
	char LoadCount=0;
	//putID
	M128_DIO_fpt(ADDR_PORT_num,ADDR_PORT_msk,ADDR_PORT_sht,ASAID);
	//put data
	*putdata_p=HEADER;
	M128_SPI_put(1,0,1, putdata_p);

	*putdata_p=CALL_TYPE_SET;
	M128_SPI_put(1,0,1, putdata_p);
	check_sum=check_sum+CALL_TYPE_SET;

	*putdata_p=LSBbyte;
	M128_SPI_put(1,0,1, putdata_p);
	check_sum=check_sum+LSBbyte;

	*putdata_p=Mask;
	M128_SPI_put(1,0,1, putdata_p);
	check_sum=check_sum+Mask;

	*putdata_p=Shift;
	M128_SPI_put(1,0,1, putdata_p);
	check_sum=check_sum+Shift;

	*putdata_p=Data;
	M128_SPI_put(1,0,1, putdata_p);
	check_sum=check_sum+Data;

	*putdata_p=check_sum;
	M128_SPI_put(1,0,1, putdata_p);

	//get return
	check_sum=0;

	while(*getdata_p!=REHEADER){
		M128_SPI_get(1,0,1,getdata_p);
		if (*getdata_p!=REHEADER){
			*getdata_p=0;
			LoadCount=LoadCount+1;
			_delay_ms(10);
		}
		if(LoadCount==1000) {  return 4; } //timeout 10s
	}
	M128_SPI_get(1,0,1,getdata_p);
	result=*getdata_p;
	check_sum=check_sum+result;

	M128_SPI_get(1,0,1,getdata_p);
	if((*getdata_p)!=check_sum){
		return 2+result;//check_sum inequal
	}
	return 0+result;
}


char ASA_SPI_put(char ASAID,char LSBbyte,char Bytes,void* Data_p){
	char check_sum=0;
	char BytesCount=0;
	char getdata=0;
	char putdata=0;
	char* getdata_p=&getdata;
	char* putdata_p=&putdata;
	char result;
	char LoadCount=0;
	//putID
	M128_DIO_fpt(ADDR_PORT_num,ADDR_PORT_msk,ADDR_PORT_sht,ASAID);
	//put data
	*putdata_p=HEADER;
	M128_SPI_put(1,0,1, putdata_p);

	*putdata_p=CALL_TYPE_PUT;
	M128_SPI_put(1,0,1, putdata_p);
	check_sum=check_sum+CALL_TYPE_PUT;

	*putdata_p=LSBbyte;
	M128_SPI_put(1,0,1, putdata_p);
	check_sum=check_sum+LSBbyte;

	*putdata_p=Bytes;
	M128_SPI_put(1,0,1, putdata_p);
	check_sum=check_sum+Bytes;

	for(BytesCount=0;BytesCount<Bytes;BytesCount++){
		*putdata_p=*((char*)Data_p+BytesCount)+BytesCount;
		M128_SPI_put(1,0,1,putdata_p);
		check_sum=check_sum+*((char*)Data_p+BytesCount)+BytesCount;
	}

	*putdata_p=check_sum;
	M128_SPI_put(1,0,1, putdata_p);

	//get return
	check_sum=0;
	while(*getdata_p!=REHEADER){
		M128_SPI_get(1,0,1,getdata_p);
		if (*getdata_p!=REHEADER){
			*getdata_p=0;
			LoadCount=LoadCount+1;
			_delay_ms(10);
		}
		if(LoadCount==1000) { return 4; }//timeout 10s
	}

	M128_SPI_get(1,0,1,getdata_p);
	result=*getdata_p;
	check_sum=check_sum+result;

	M128_SPI_get(1,0,1,getdata_p);
	if((*getdata_p)!=check_sum){
		return 2+result;//check_sum inequal
	}
	return 0+result;
}

char ASA_SPI_get(char ASAID,char LSBbyte,char Bytes,void* Data_p){
	char check_sum=0;
	char getdata=0;
	char* getdata_p=&getdata;
	char putdata=0;
	char* putdata_p=&putdata;
	char result;
	char LoadCount=0;

	//putID
	M128_DIO_fpt(ADDR_PORT_num,ADDR_PORT_msk,ADDR_PORT_sht,ASAID);
	//put data
	*putdata_p=HEADER;
	M128_SPI_put(1,0,1, putdata_p);

	*putdata_p=CALL_TYPE_GET;
	M128_SPI_put(1,0,1, putdata_p);
	check_sum=check_sum+CALL_TYPE_GET;

	*putdata_p=LSBbyte;
	M128_SPI_put(1,0,1, putdata_p);
	check_sum=check_sum+LSBbyte;

	*putdata_p=Bytes;
	M128_SPI_put(1,0,1, putdata_p);
	check_sum=check_sum+Bytes;

	*putdata_p=check_sum;
	M128_SPI_put(1,0,1, putdata_p);

	//get return
	check_sum=0;
	while(*getdata_p!=REHEADER){
		M128_SPI_get(1,0,1,getdata_p);
		if (*getdata_p!=REHEADER){
			LoadCount=LoadCount+1;
			_delay_ms(10);
		}
		if(LoadCount==1000) { return 4; }//timeout 10s
	}

	M128_SPI_get(1,0,1,getdata_p);
	check_sum=check_sum+*getdata_p;
	result=*getdata_p;

	if(result==0){
		for (int BytesCount = 0; BytesCount < Bytes; BytesCount++){
			M128_SPI_get(1,0,1,getdata_p);
			check_sum=check_sum+*getdata_p;
			*((char*)Data_p +BytesCount)=*getdata_p-BytesCount;
		}
	}

	M128_SPI_get(1,0,1,getdata_p);
	if(*(getdata_p)!=check_sum){return 2+result;}
	return 0+result;
}

char ASA_SPI_fpt(char ASAID,char LSBbyte,char Mask,char Shift,char Data){
	char check_sum=0;
	char getdata=0;
	char* getdata_p=&getdata;
	char putdata=0;
	char* putdata_p=&putdata;
	char result;
	char LoadCount=0;
	//putID
	M128_DIO_fpt(ADDR_PORT_num,ADDR_PORT_msk,ADDR_PORT_sht,ASAID);
	//put data
	*putdata_p=HEADER;
	M128_SPI_put(1,0,1, putdata_p);

	*putdata_p=CALL_TYPE_FPT;
	M128_SPI_put(1,0,1, putdata_p);
	check_sum=check_sum+CALL_TYPE_FPT;

	*putdata_p=LSBbyte;
	M128_SPI_put(1,0,1, putdata_p);
	check_sum=check_sum+LSBbyte;

	*putdata_p=Mask;
	M128_SPI_put(1,0,1, putdata_p);
	check_sum=check_sum+Mask;

	*putdata_p=Shift;
	M128_SPI_put(1,0,1, putdata_p);
	check_sum=check_sum+Shift;

	*putdata_p=Data;
	M128_SPI_put(1,0,1, putdata_p);
	check_sum=check_sum+Data;

	*putdata_p=check_sum;
	M128_SPI_put(1,0,1, putdata_p);

	//get return
	check_sum=0;
	while(*getdata_p!=REHEADER){
		M128_SPI_get(1,0,1,getdata_p);
		if (*getdata_p!=REHEADER){
			LoadCount=LoadCount+1;
			_delay_ms(10);
		}
		if(LoadCount==1000) { return 4; }//timeout 10s
	}
	M128_SPI_get(1,0,1,getdata_p);
	result=*getdata_p;
	check_sum=check_sum+result;
	M128_SPI_get(1,0,1,getdata_p);
	if(*(getdata_p)!=check_sum){return 2+result;}//check_sum inequal
	return 0+result;
}

char ASA_SPI_fgt(char ASAID,char LSBbyte,char Mask,char Shift,char* Data_p){
	char check_sum=0;
	char getdata=0;
	char* getdata_p=&getdata;
	char putdata=0;
	char* putdata_p=&putdata;
	char result;
	char LoadCount=0;
	//putID
	M128_DIO_fpt(ADDR_PORT_num,ADDR_PORT_msk,ADDR_PORT_sht,ASAID);
	//put data
	*putdata_p=HEADER;
	M128_SPI_put(1,0,1, putdata_p);

	*putdata_p=CALL_TYPE_FGT;
	M128_SPI_put(1,0,1, putdata_p);
	check_sum=check_sum+CALL_TYPE_FGT;

	*putdata_p=LSBbyte;
	M128_SPI_put(1,0,1, putdata_p);
	check_sum=check_sum+LSBbyte;

	*putdata_p=Mask;
	M128_SPI_put(1,0,1, putdata_p);
	check_sum=check_sum+Mask;

	*putdata_p=Shift;
	M128_SPI_put(1,0,1, putdata_p);
	check_sum=check_sum+Shift;

	*putdata_p=check_sum;
	M128_SPI_put(1,0,1, putdata_p);

	//get return
	check_sum=0;
	while(*getdata_p!=REHEADER){
		M128_SPI_get(1,0,1,getdata_p);
		if (*getdata_p!=REHEADER){
			LoadCount=LoadCount+1;
			_delay_ms(10);
		}
		if(LoadCount==1000) { return 4; }//timeout 10s
	}
	M128_SPI_get(1,0,1,getdata_p);
	result=*getdata_p;
	check_sum=check_sum+result;
	if(result==0){
		M128_SPI_get(1,0,1,getdata_p);
		*(Data_p)=(*getdata_p);
		check_sum=check_sum+(*getdata_p);
	}
	M128_SPI_get(1,0,1,getdata_p);
	if((*getdata_p)!=check_sum){return 2+result;}
	return 0+result;
}