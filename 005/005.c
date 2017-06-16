#include "..\LLL\ASA_Lib.h"
#include "..\LLL\ASA_DIO.h"

#define ASA_ID 1
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
#define TWI_STATUS	(TWSR & 0xF8)
//Internal Definitions
#define THM01_Put_TmpReg			0x00		//Pointer Register for Temperature Register of Tmp175
#define THM01_Set_ConfgReg			0x01		//Configuration Register for Temperature Register of Tmp175
#define THM01_Set_Format			0x60


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
	//bits_put((1<<TWINT)|(1 << TWEN), TWCR, (1<<TWINT)|(1 << TWEN)|(1<<TWSTA), 0);
	return 0;
}
char M128_TWI_fgt( char LSByte, char Mask,  char shift,  char* Data_p){
	if (LSByte!=201) return 1;
	if (Mask<0 || Mask>255) return 1;
	if(shift<0 || shift>7) return 1;
	//感覺怪怪的
	*Data_p=bits_get(TWSR, Mask, shift);
	return 0;
}

void TWI_Stop_n(void){
	printf("TWI_stop\n");
	TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWSTO);
}

char TWI_Start_n(void){
	printf("TWI_start\n");
	TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWSTA);
	while (!(TWCR & (1<<TWINT)));
	if (TWI_STATUS != TWI_Start){
		printf("TWI_start err\n");
		TWI_Stop_n();
		return 1;
	}
	else
		return 0;
}
char TWI_ReStart_n(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	if (TWI_STATUS != TWI_REP_START)
	{
		TWI_Stop_n();
		return 2;
	}
	else
		return 0;
}
void TWI_send(char data){
	printf("TWI_send\n");
	TWDR = data;
	TWCR = (1<<TWINT)|(1<<TWEN);
	//M128_TWI_fpt(202 , (1<<TWINT)|(1 << TWEN)|(1 << TWSTA) , 0 , 0x84);
	while( !(bit_check(TWCR,TWINT)) );
	printf("TWI_send y\n");
}



char TWI_TMP175_set(void){
	unsigned int SLA_ADD;
	SLA_ADD = 0b10010000;
	char TWI_status;

	unsigned int SLA_W = SLA_ADD & 0XFE;

	M128_TWI_fpt(202 , (1<<TWINT)|(1 << TWEN)|(1<<TWSTA) , 0 , (1<<TWINT)|(1 << TWEN)|(1<<TWSTA));
	while( !(bit_check(TWCR,TWINT)) );
	M128_TWI_fgt(201 , 0xf8 , 0 , &TWI_status);
	if (TWI_status != TWI_Start){
		//stop
		M128_TWI_fpt(202 , (1 << TWINT)|(1<<TWSTA)|(1 << TWSTO) , 0 , (1 << TWINT)|(1 << TWSTO));
		return 1;
	}
	printf("start check\n");

	//sent SLA_W
	M128_TWI_put(0,1,(char*)&SLA_W);

	M128_TWI_fpt(202 , ((1<<TWINT)|(1 << TWEN)|(1<<TWSTA)) , 0 , ((1<<TWINT)|(1 << TWEN)));
	printf("TWCR 1  %x\n",TWCR );
	while( !(bit_check(TWCR,TWINT)) );
	printf("TWCR 2  %x\n",TWCR );
	//get ack, should be TWI_MT_SLA_ACK
	M128_TWI_fgt(201 , 0xf8 , 0 , &TWI_status);
	if (TWI_status != TWI_MT_SLA_ACK){
		//stop
		printf("%x\n",TWI_status);
		M128_TWI_fpt(202 , (1 << TWINT)|(1 << TWEN)|(1<<TWSTA)|(1 << TWSTO) , 0 , 255);
		return 2;
	}
	printf("sla check in\n");
//------------------------check---------------------------
	TWI_send(THM01_Set_ConfgReg);
	if (TWI_STATUS != TWI_MT_DATA_ACK){
		printf("data1 ack err\n");
		TWI_Stop_n();
		return 3;
	}
	TWI_send(THM01_Set_Format);
	if (TWI_STATUS != TWI_MT_DATA_ACK){
		printf("data2 ack err\n");
		TWI_Stop_n();
		return 3;
	}
	TWI_Stop_n();
	return 0;
}

char TWI_TMP175_get(void* Data_p){
	unsigned int SLA_ADD;
	SLA_ADD = 0b10010000;
	unsigned int SLA_W = SLA_ADD & 0XFE;
	unsigned int SLA_R = SLA_ADD | 0X01;
	char datatemp[2];
	float tmp;
	printf("get begin\n");
	TWI_Start_n();
	TWI_send(SLA_W);
	if (TWI_STATUS != TWI_MT_SLA_ACK){
		printf("sla ack err\n");
		TWI_Stop_n();
		return 3;
	}
	TWI_send(THM01_Put_TmpReg);
	if (TWI_STATUS != TWI_MT_DATA_ACK){
		printf("data put err\n");
		TWI_Stop_n();
		return 3;
	}

	TWI_ReStart_n();
	TWI_send(SLA_R);
	if (TWI_STATUS != TWI_MR_SLA_ACK){
		printf("sla r err\n");
		TWI_Stop_n();
		return 3;
	}

	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	while (!(TWCR & (1<<TWINT)));
	if (TWI_STATUS != TWI_MR_DATA_ACK){
		printf("g data 1 err\n");
		printf("%x\n", TWI_STATUS);
		TWI_Stop_n();
		return 6;
	}
	datatemp[0] = TWDR;
	printf("data1  %d\n", (int)datatemp[0]);

	TWCR = (1<<TWINT)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	if (TWI_STATUS != TWI_MR_DATA_NACK){
		printf("g data 2 err\n");
		TWI_Stop_n();
		return 7;
	}
	datatemp[1] = TWDR;
	printf("data2  %d\n", (int)datatemp[1]);
	TWCR = (1<<TWINT)|(1<<TWEN);
	TWI_Stop_n();
	tmp = ((datatemp[0]<<8)+datatemp[1])>>4;
	tmp = tmp*0.0625;
	printf("final tmp  %d\n",(int)tmp );
	*((float*)Data_p)=tmp;
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

int main(){
	ASA_M128_set();
	M128_ASAID_SET(ASA_ID);
	M128_TWI_set(200,0xff,0,0xc8);
	float tmpdata;
	char check;
	unsigned int SLA_ADD;
	SLA_ADD = 0b10010000;
	char data;
	data=THM01_Set_Format;
	printf("------------------------------------------\n");
	check=M128_TWI_trm(4, SLA_ADD, THM01_Set_ConfgReg, 1 ,&data);
	printf("%d\n",check );
	TWI_TMP175_get(&tmpdata);
	printf("main tmp  %d\n", (int)tmpdata);

	while(1){}
	return 0;
}

