#include "..\LLL\ASA_TWI.h"

void M128_TWI_ini(void){
	// note: TWBR of master should not less than 10
	M128_TWI_set(200, 0xff , 0 ,0x10);
}

char M128_TWI_set( char LSByte, char Mask,  char shift,  char Data){
	if (LSByte<200 || LSByte>203) return 1;
	if (Mask<0 || Mask>255) return 1;
	if(shift<0 || shift>7) return 1;
	switch(LSByte){
		case 200:
			bits_put(Data, TWBR, Mask, shift);
			break;
		case 201:
			bits_put(Data, TWSR, Mask, shift);
			break;
		case 202:
			bits_put(Data, TWCR, Mask, shift);
			break;
		case 203:
			bits_put(Data, TWAR, Mask, shift);
			break;
	}
	return 0;
}

char M128_TWI_put( char LSByte, char Bytes, char* Data_p){
	if(LSByte!=0) return 1;
	if(Bytes!=1) return 1;
	TWDR = *Data_p;
	return 0;
}

char M128_TWI_get( char LSByte, char Bytes, char* Data_p){
	if(LSByte!=0) return 1;
	if(Bytes!=1) return 1;
	*Data_p = TWDR;
	return 0;
}

char M128_TWI_fpt( char LSByte, char Mask,  char shift,  char Data){
	if (LSByte!=202) return 1;
	if (Mask<0 || Mask>255) return 1;
	if(shift<0 || shift>7) return 1;
	bits_put(Data, TWCR, Mask, shift);
	//原版bits_put會蓋寫dest兩次 TWINT會寫0再寫1 觸發未知的錯誤---------------------------------
	return 0;
}

char M128_TWI_fgt( char LSByte, char Mask,  char shift,  char* Data_p){
	if (LSByte!=201) return 1;
	if (Mask<0 || Mask>255) return 1;
	if(shift<0 || shift>7) return 1;
	*Data_p=bits_get(TWSR, Mask, shift);
	return 0;
}

void TWI_movement_Stop(void){
	//stop & clear TWSTA
	M128_TWI_fpt(202 , (1 << TWINT)|(1 << TWEN)|(1 << TWSTO)|(1<<TWSTA) , 0 , (1 << TWINT)|(1 << TWEN)|(1 << TWSTO));
}

char TWI_movement_Start(void){
	char TWI_status;
	M128_TWI_fpt(202 , (1<<TWINT)|(1 << TWEN)|(1<<TWSTA) , 0 , (1<<TWINT)|(1 << TWEN)|(1<<TWSTA));
	while( !(bit_check(TWCR,TWINT)) );
	//get ack, should be TWI_Start
	M128_TWI_fgt(201 , 0xf8 , 0 , &TWI_status);
	if (TWI_status != TWI_Start){
		TWI_movement_Stop();
		return TWI_status;
	}
	return 0;
}

char TWI_movement_Restart(void){
	char TWI_status;
	M128_TWI_fpt(202 , (1<<TWINT)|(1 << TWEN)|(1<<TWSTA) , 0 , (1<<TWINT)|(1 << TWEN)|(1<<TWSTA));
	while( !(bit_check(TWCR,TWINT)) );
	//get ack, should be TWI_REP_START
	M128_TWI_fgt(201 , 0xf8 , 0 , &TWI_status);
	if (TWI_status != TWI_REP_START){
		TWI_movement_Stop();
		return TWI_status;
	}
	return 0;
}

char TWI_movement_MSLA_W(void){
	char TWI_status;
	M128_TWI_fpt(202 , ((1<<TWINT)|(1 << TWEN)|(1<<TWSTA)) , 0 , ((1<<TWINT)|(1 << TWEN)));
	while( !(bit_check(TWCR,TWINT)) );
	//get ack, should be TWI_MT_SLA_ACK
	M128_TWI_fgt(201 , 0xf8 , 0 , &TWI_status);
	if (TWI_status != TWI_MT_SLA_ACK){
		TWI_movement_Stop();
		return TWI_status;
	}
	return 0;
}

char TWI_movement_MSLA_R(void){
	char TWI_status;
	M128_TWI_fpt(202 , ((1<<TWINT)|(1 << TWEN)|(1<<TWSTA)) , 0 , ((1<<TWINT)|(1 << TWEN)));
	while( !(bit_check(TWCR,TWINT)) );
	//get ack, should be TWI_MR_SLA_ACK
	M128_TWI_fgt(201 , 0xf8 , 0 , &TWI_status);
	if (TWI_status != TWI_MR_SLA_ACK){
		TWI_movement_Stop();
		return TWI_status;
	}
	return 0;
}

char TWI_movement_MDataTransmit(void){
	char TWI_status;
	M128_TWI_fpt(202 , (1<<TWINT)|(1 << TWEN) , 0 , (1<<TWINT)|(1 << TWEN));
	while( !(bit_check(TWCR,TWINT)) );
	//get ack, should be TWI_MT_DATA_ACK
	M128_TWI_fgt(201 , 0xf8 , 0 , &TWI_status);
	if (TWI_status != TWI_MT_DATA_ACK){
		TWI_movement_Stop();
		return TWI_status;
	}
	return 0;
}

char TWI_movement_MDataReceive_nLast(void){
	char TWI_status;
	M128_TWI_fpt(202 , (1<<TWINT)|(1<<TWEN)|(1<<TWEA) , 0 , (1<<TWINT)|(1<<TWEN)|(1<<TWEA));
	while( !(bit_check(TWCR,TWINT)) );
	//get ack, should be TWI_MR_DATA_ACK
	M128_TWI_fgt(201 , 0xf8 , 0 , &TWI_status);
	if (TWI_status != TWI_MR_DATA_ACK){
		TWI_movement_Stop();
		return TWI_status;
	}
	return 0;
}

char TWI_movement_MDataReceive_Last(void){
	char TWI_status;
	M128_TWI_fpt(202 , (1<<TWINT)|(1<<TWEN)|(1<<TWEA) , 0 , (1<<TWINT)|(1<<TWEN));
	while( !(bit_check(TWCR,TWINT)) );
	//get ack, should be TWI_MR_DATA_NACK
	M128_TWI_fgt(201 , 0xf8 , 0 , &TWI_status);
	if (TWI_status != TWI_MR_DATA_NACK){
		TWI_movement_Stop();
		return TWI_status;
	}
	return 0;
}

char M128_TWI_trm(char OneReg, char SLA, char RegAdd, char Bytes, void *Data_p){
	unsigned int SLA_W = SLA & 0XFE;
	char check;
	//start
	check=TWI_movement_Start();
	if(check!=0) return 1;

	//sent SLA_W
	M128_TWI_put(0,1,(char*)&SLA_W);
	check=TWI_movement_MSLA_W();
	if(check!=0) return 2;

	if(OneReg==1){
		for(int i=0 ; i<Bytes ; i++){
			//sent data
			M128_TWI_put(0,1,Data_p+i);
			check=TWI_movement_MDataTransmit();
			if(check!=0){
				return 4;
			}
		}
		TWI_movement_Stop();
		return 0;
	}

	else{
		//sent RegAdd
		M128_TWI_put(0, 1,&RegAdd);
		check=TWI_movement_MDataTransmit();
		if(check!=0) return 3;
		for(int i=0 ; i<Bytes ; i++){
			//sent data
			M128_TWI_put(0,1,Data_p+i);
			check=TWI_movement_MDataTransmit();
			if(check!=0) return 4;
		}
		TWI_movement_Stop();
		return 0;
	}
}
char M128_TWI_rec(char OneReg, char SLA, char RegAdd, char Bytes, void *Data_p){
	unsigned int SLA_W = SLA & 0XFE;
	unsigned int SLA_R = SLA | 0X01;
	char check;
	//start
	check=TWI_movement_Start();
	if(check!=0) return 1;

	if(OneReg==1){
		//sent SLA_R
		M128_TWI_put(0,1,(char*)&SLA_R);
		check=TWI_movement_MSLA_R();
		if(check!=0) return 5;
		//Receive data
		for(int i=0 ; i<Bytes-1 ; i++){
			check=TWI_movement_MDataReceive_nLast();
			if(check!=0) return 6;
			M128_TWI_get( 0, 1, (char*)Data_p+i);
		}
		//Receive lastdata
		check=TWI_movement_MDataReceive_Last();
		if(check!=0) return 6;
		M128_TWI_get( 0, 1, (char*)Data_p+Bytes-1);
		TWI_movement_Stop();
		return 0;
	}

	else {
		//sent SLA_W
		M128_TWI_put(0,1,(char*)&SLA_W);
		check=TWI_movement_MSLA_W();
		if(check!=0) return 2;
		//sent RegAdd
		M128_TWI_put(0, 1,&RegAdd);
		check=TWI_movement_MDataTransmit();
		if(check!=0) return 3;
		//Restart
		check=TWI_movement_Restart();
		if(check!=0) return 4;
		//sent SLA_R
		M128_TWI_put(0,1,(char*)&SLA_R);
		check=TWI_movement_MSLA_R();
		if(check!=0) return 5;
		//Receive data
		for(int i=0 ; i<Bytes-1 ; i++){
			check=TWI_movement_MDataReceive_nLast();
			if(check!=0) return 6;
			M128_TWI_get( 0, 1, (char*)Data_p+i);
		}
		//Receive lastdata
		check=TWI_movement_MDataReceive_Last();
		if(check!=0) return 6;
		M128_TWI_get( 0, 1, (char*)Data_p+Bytes-1);
		TWI_movement_Stop();
		return 0;
	}
}


char M128_TWI_rec_forasa( char SLA, char Bytes, char* result, char* check_sum, void *Data_p){
	unsigned int SLA_R = SLA | 0X01;
	char check;
	char gdata;
	char* gdata_p=&gdata;
	char forRe=0;
	//start
	check=TWI_movement_Restart();
	if(check!=0) return 1;
	//sent SLA_R
	M128_TWI_put(0,1,(char*)&SLA_R);
	check=TWI_movement_MSLA_R();
	if(check!=0) return 5;
	//receive reheader
	while(forRe!=REHEADER){
		check=TWI_movement_MDataReceive_nLast();
		if(check!=0) return 6;
		M128_TWI_get( 0, 1, &forRe);
	}
	//_delay_ms(1);
	//Receive result
	check=TWI_movement_MDataReceive_nLast();
	if(check!=0) return 6;
	M128_TWI_get( 0, 1, result);
	//printf("%d\n",*(result) );
	//_delay_ms(1);

	if(*(result) == 0){
		//Receive data
		for(int i=0 ; i<Bytes ; ++i){
			check=TWI_movement_MDataReceive_nLast();
			if(check!=0) return 6;
			M128_TWI_get( 0, 1, gdata_p);
			//printf("%d\n",*gdata_p );
			*((char*)Data_p+i)=gdata-i;
			//_delay_ms(1);
		}
	}

	//Receive check_sum
	check=TWI_movement_MDataReceive_Last();
	if(check!=0) return 6;
	M128_TWI_get( 0, 1, check_sum);
	//printf("%d\n",*(check_sum) );
	TWI_movement_Stop();
	return 0;
}

char ASA_TWI_set(char ASAID, char SLA, char LSBbyte, char Mask, char Shift, char Data){
	char check_sum=0;
	char gcheck_sum=0;
	char check=0;
	unsigned int SLA_W = SLA & 0XFE;
	char putdata;
	char* putdata_p=&putdata;
	char result;
	M128_ASAID_SET(ASAID);

	check=TWI_movement_Start();
	if(check!=0) return 4;

	//sent SLA_W
	M128_TWI_put(0,1,(char*)&SLA_W);
	check=TWI_movement_MSLA_W();
	if(check!=0) return 5;

	putdata=HEADER;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	putdata=CALL_TYPE_SET;
	check_sum=check_sum+putdata;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	putdata=LSBbyte;
	check_sum=check_sum+putdata;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	putdata=Mask;
	check_sum=check_sum+putdata;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	putdata=Shift;
	check_sum=check_sum+putdata;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	putdata=Data;
	check_sum=check_sum+putdata;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	putdata=check_sum;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	check_sum=0;
	check=M128_TWI_rec_forasa( SLA, 0, &result, &gcheck_sum, NULL);
	if(check!=0) return 7;
	check_sum=check_sum+result;
	if(check_sum!=gcheck_sum){
		return 2+result;//check_sum inequal
	}
	return 0+result;
}

char ASA_TWI_put(char ASAID, char SLA,char LSBbyte,char Bytes,void* Data_p){
	char check_sum=0;
	char gcheck_sum=0;
	unsigned int SLA_W = SLA & 0XFE;
	char check=0;
	char putdata;
	char* putdata_p=&putdata;
	char result;
	M128_ASAID_SET(ASAID);
	check=TWI_movement_Start();
	if(check!=0) return 4;

	//sent SLA_W
	M128_TWI_put(0,1,(char*)&SLA_W);
	check=TWI_movement_MSLA_W();
	if(check!=0) return 5;

	putdata=HEADER;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	putdata=CALL_TYPE_PUT;
	check_sum=check_sum+putdata;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	putdata=LSBbyte;
	check_sum=check_sum+putdata;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	putdata=Bytes;
	check_sum=check_sum+putdata;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	for (int i=0; i < Bytes; ++i){
		putdata=*((char*)Data_p+i)+(char)i;
		check_sum=check_sum+putdata;
		M128_TWI_put(0,1,putdata_p);
		check=TWI_movement_MDataTransmit();
		if(check!=0)return 6;
	}

	putdata=check_sum;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	check_sum=0;
	check=M128_TWI_rec_forasa( SLA, 0, &result, &gcheck_sum, NULL);
	if(check!=0) return 5;
	check_sum=check_sum+result;
	if(check_sum!=gcheck_sum){
		return 2+result;//check_sum inequal
	}
	return 0+result;
}

char ASA_TWI_get(char ASAID, char SLA, char LSBbyte, char Bytes,void* Data_p){
	char check_sum=0;
	char gcheck_sum=0;
	unsigned int SLA_W = SLA & 0XFE;
	char check=0;
	char putdata;
	char* putdata_p=&putdata;
	char result;
	M128_ASAID_SET(ASAID);
	check=TWI_movement_Start();
	if(check!=0) return 4;

	//sent SLA_W
	M128_TWI_put(0,1,(char*)&SLA_W);
	check=TWI_movement_MSLA_W();
	if(check!=0) return 5;

	putdata=HEADER;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	putdata=CALL_TYPE_GET;
	check_sum=check_sum+putdata;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	putdata=LSBbyte;
	check_sum=check_sum+putdata;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	putdata=Bytes;
	check_sum=check_sum+putdata;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	putdata=check_sum;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	check_sum=0;
	check=M128_TWI_rec_forasa( SLA, Bytes, &result, &gcheck_sum, Data_p);
	if(check!=0) return 5;
	check_sum=check_sum+result;
	if(result==0){
		for (int i = 0; i < Bytes; ++i){
			check_sum=check_sum+*((char*)Data_p+i)+i;
		}
	}
	if(check_sum!=gcheck_sum){
		return 2+result;//check_sum inequal
	}
	return 0+result;
}

char ASA_TWI_fpt(char ASAID, char SLA, char LSBbyte, char Mask,char Shift,char Data){
	char check_sum=0;
	char gcheck_sum=0;
	char check=0;
	unsigned int SLA_W = SLA & 0XFE;
	char putdata;
	char* putdata_p=&putdata;
	char result;
	M128_ASAID_SET(ASAID);

	check=TWI_movement_Start();
	if(check!=0) return 4;

	//sent SLA_W
	M128_TWI_put(0,1,(char*)&SLA_W);
	check=TWI_movement_MSLA_W();
	if(check!=0) return 5;

	putdata=HEADER;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	putdata=CALL_TYPE_FPT;
	check_sum=check_sum+putdata;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	putdata=LSBbyte;
	check_sum=check_sum+putdata;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	putdata=Mask;
	check_sum=check_sum+putdata;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	putdata=Shift;
	check_sum=check_sum+putdata;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	putdata=Data;
	check_sum=check_sum+putdata;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	putdata=check_sum;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	check_sum=0;
	check=M128_TWI_rec_forasa( SLA, 0, &result, &gcheck_sum, NULL);
	if(check!=0) return 7;
	check_sum=check_sum+result;
	if(check_sum!=gcheck_sum){
		return 2+result;//check_sum inequal
	}
	return 0+result;
}

char ASA_TWI_fgt(char ASAID, char SLA, char LSBbyte, char Mask,char Shift,char* Data_p){
	char check_sum=0;
	char gcheck_sum=0;
	char check=0;
	unsigned int SLA_W = SLA & 0XFE;
	char putdata;
	char* putdata_p=&putdata;
	char result;

	M128_ASAID_SET(ASAID);

	check=TWI_movement_Start();
	if(check!=0) return 4;

	//sent SLA_W
	M128_TWI_put(0,1,(char*)&SLA_W);
	check=TWI_movement_MSLA_W();
	if(check!=0) return 5;

	putdata=HEADER;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	putdata=CALL_TYPE_FGT;
	check_sum=check_sum+putdata;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	putdata=LSBbyte;
	check_sum=check_sum+putdata;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	putdata=Mask;
	check_sum=check_sum+putdata;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	putdata=Shift;
	check_sum=check_sum+putdata;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	putdata=check_sum;
	M128_TWI_put(0,1,putdata_p);
	check=TWI_movement_MDataTransmit();
	if(check!=0)return 6;

	check_sum=0;
	check=M128_TWI_rec_forasa( SLA, 1, &result, &gcheck_sum, Data_p);
	if(check!=0)return 5;

	if(result==0){
		check_sum=check_sum+*((char*)Data_p);
	}

	if(check_sum!=gcheck_sum){
		return 2+result;//check_sum inequal
	}
	return 0+result;
}