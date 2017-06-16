
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


char ASA_SPI_set(char ASAID,char LSBbyte,char Mask,char Shift,char Data);
char ASA_SPI_put(char ASAID,char LSBbyte,char Bytes,void* Data_p);
char ASA_SPI_get(char ASAID,char LSBbyte,char Bytes,void* Data_p);
char ASA_SPI_fpt(char ASAID,char LSBbyte,char Mask,char Shift,char Data);
char ASA_SPI_fgt(char ASAID,char LSBbyte,char Mask,char Shift,char* Data_p);

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
	result=*getdata_p;
	check_sum=check_sum+result;
	if(result==0){
		for (int BytesCount = 0; BytesCount < Bytes; BytesCount++){
			M128_SPI_get(1,0,1,getdata_p);
			*((char*)Data_p +BytesCount)=(*getdata_p)-BytesCount;
			check_sum=check_sum+(*getdata_p);
		}
	}
	M128_SPI_get(1,0,1,getdata_p);
	if((*getdata_p)!=check_sum){return 2+result;}
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
	if((*getdata_p)!=check_sum){return 2+result;}//check_sum inequal
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
