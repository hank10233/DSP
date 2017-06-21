#include "..\LLL\ASA_TWI.h"

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
printf("start check\n");
	//sent SLA_W
	M128_TWI_put(0,1,(char*)&SLA_W);
	check=TWI_movement_MSLA_W();
	if(check!=0) return 2;
printf("sla check in\n");

	if(OneReg==1){
		for(int i=0 ; i<Bytes ; i++){
			//sent data
			M128_TWI_put(0,1,Data_p+i);
			check=TWI_movement_MDataTransmit();
			if(check!=0) return 4;
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