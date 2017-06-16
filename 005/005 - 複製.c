#include "..\LLL\ASA_Lib.h"
#include "..\LLL\ASA_DIO.h"

#define TWI_Start 0x08
#define TWI_REP_START		0x10
/*Status codes for master transmitter mode*/
#define TWI_MT_SLA_ACK		0x18
#define TWI_MT_SLA_NACK		0x20
#define TWI_MT_DATA_ACK		0x28
#define TWI_MT_DATA_NACK	0x30
#define TWI_MT_ARB_LOST		0x38
/*Status codes for master receiver mode*/
#define TWI_MR_SLA_ACK		0x40
#define TWI_MR_SLA_NACK		0x48
#define TWI_MR_DATA_ACK		0x50
#define TWI_MR_DATA_NACK	0x58
#define TWI_MR_ARB_LOST	    0x38

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

char M128_TWI_trm(char OneReg, char SLA, char RegAdd, char Bytes, void *Data_p){
	unsigned int SLA_W = SLA & 0XFE;
	char TWI_status;
	//start
	M128_TWI_fpt(202 , (1<<TWINT)|(1 << TWEN)|(1<<TWSTA) , 0 , (1<<TWINT)|(1 << TWEN)|(1<<TWSTA));
	while( !(bit_check(TWCR,TWINT)) );
	//get ack, should be TWI_Start
	M128_TWI_fgt(201 , 0xf8 , 0 , &TWI_status);
	if (TWI_status != TWI_Start){
		//stop  clear TWSTA
		M128_TWI_fpt(202 , (1 << TWINT)|(1 << TWEN)|(1 << TWSTO)|(1<<TWSTA) , 0 , (1 << TWINT)|(1 << TWEN)|(1 << TWSTO));
		return 1;
	}
	//sent SLA_W clear TWSTA
	M128_TWI_put(0,1,&SLA_W);
	M128_TWI_fpt(202 , (1<<TWINT)|(1 << TWEN)|(1<<TWSTA) , 0 , (1<<TWINT)|(1 << TWEN));
	while( !(bit_check(TWCR,TWINT)) );
	//get ack, should be TWI_MT_SLA_ACK
	M128_TWI_fgt(201 , 0xf8 , 0 , &TWI_status);
	if (TWI_status != TWI_MT_SLA_ACK){
		//stop
		M128_TWI_fpt(202 , (1 << TWINT)|(1 << TWEN)|(1 << TWSTO) , 0 , (1 << TWINT)|(1 << TWEN)|(1 << TWSTO));
		return 2;
	}
	if(OneReg==1){
		for(int i=0 ; i<Bytes ; i++){
			//sent data
			M128_TWI_put(0,1,Data_p+i);
			M128_TWI_fpt(202 , (1<<TWINT)|(1 << TWEN) , 0 , (1<<TWINT)|(1 << TWEN));
			while( !(bit_check(TWCR,TWINT)) );
			//get ack, should be TWI_MT_DATA_ACK
			M128_TWI_fgt(201 , 0xf8 , 0 , &TWI_status);
			if (TWI_status != TWI_MT_DATA_ACK){
				//stop
				M128_TWI_fpt(202 , (1 << TWINT)|(1 << TWEN)|(1 << TWSTO) , 0 , (1 << TWINT)|(1 << TWEN)|(1 << TWSTO));
				return 3;
			}
		}
		//stop
		M128_TWI_fpt(202 , (1 << TWINT)|(1 << TWEN)|(1 << TWSTO) , 0 , (1 << TWINT)|(1 << TWEN)|(1 << TWSTO));
		return 0;
	}
	else{
		//sent RegAdd
		M128_TWI_put(0, 1,&RegAdd);
		M128_TWI_fpt(202 , (1<<TWINT)|(1 << TWEN) , 0 , (1<<TWINT)|(1 << TWEN));
		while( !(bit_check(TWCR,TWINT)) );
		//get ack, should be TWI_MT_DATA_ACK
		M128_TWI_fgt(201 , 0xf8 , 0 , &TWI_status);
		if (TWI_status != TWI_MT_DATA_ACK){
			//stop
			M128_TWI_fpt(202 , (1 << TWINT)|(1 << TWEN)|(1 << TWSTO) , 0 , (1 << TWINT)|(1 << TWEN)|(1 << TWSTO));
			return 4;
		}
		for(int i=0 ; i<Bytes ; i++){
			//sent data
			M128_TWI_put(0,1,Data_p+i);
			M128_TWI_fpt(202 , (1<<TWINT)|(1 << TWEN) , 0 , (1<<TWINT)|(1 << TWEN));
			while( !(bit_check(TWCR,TWINT)) );
			//get ack, should be TWI_MT_DATA_ACK
			M128_TWI_fgt(201 , 0xf8 , 0 , &TWI_status);
			if (TWI_status != TWI_MT_DATA_ACK){
				//stop
				M128_TWI_fpt(202 , (1 << TWINT)|(1 << TWEN)|(1 << TWSTO) , 0 , (1 << TWINT)|(1 << TWEN)|(1 << TWSTO));
				return 3;
			}
		}
		//stop
		M128_TWI_fpt(202 , (1 << TWINT)|(1 << TWEN)|(1 << TWSTO) , 0 , (1 << TWINT)|(1 << TWEN)|(1 << TWSTO));
		return 0;
	}
}

char M128_TWI_rec(char OneREG, char SLA, char RegAdd, char Bytes, void *Data_p){

}

int main (void){
	ASA_M128_set();
	M128_ASAID_SET(ASA_ID);
	M128_TWI_set(200,0xff,0,0xc8);
	M128_TWI_trm()
}