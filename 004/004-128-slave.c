#include "ASA_Lib.h"
#include "ASA_SPI.h"
#include <stdlib.h>

#define MAXBUFFBYTES 32
#define HEADER 0x87
#define REHEADER 0x88
#define receiving 0
#define transmitting 1
#define CALL_TYPE_SET 0
#define CALL_TYPE_PUT 1
#define CALL_TYPE_GET 2
#define CALL_TYPE_FPT 3
#define CALL_TYPE_FGT 4
#define STATUS_HEADER 0
#define STATUS_CALLTYPE 1
#define STATUS_LSBYTE 2
#define STATUS_BYTES 3
#define STATUS_MASK 4
#define STATUS_SHIFT 5
#define STATUS_DATA 6
#define STATUS_CHECKSUM 7

char slave_set(char LSByte,char Mask,char Shift,char Data);
char slave_put(char LSByte,char Bytes,void*  Data_p);
char slave_get(char LSByte,char Bytes,void*  Data_p);
char slave_fpt(char LSByte,char Mask,char Shift,char Data);
char slave_fgt(char LSByte,char Mask,char Shift,char *Data);

char TestData[2];
volatile int count_ISR=0;
volatile char* Pindex_p;

//緩衝區資料結構型態
typedef struct {
char PUTindex;
char GETindex;
char data[MAXBUFFBYTES];
} TypeOfBuffer;

TypeOfBuffer Com_Buff_str;
TypeOfBuffer Res_Buff_str;

//SPI通訊處理用資料結構
typedef struct {
char status;
TypeOfBuffer* InBUFF_p;
TypeOfBuffer* OutBUFF_p;
} TypeOfslave_SPI_swap;

TypeOfslave_SPI_swap slave_SPI_swap_str;

//通訊封包處理及回應產生器用資料結構
typedef struct {
char status;
TypeOfBuffer* InBUFF_p;
TypeOfBuffer* OutBUFF_p;
char CallType;
char LSByte;
char Bytes;
char Mask;
char Shift;
char CheckSum;
void* Data_p;
//char Data[MAXBUFFBYTES];
//Data若宣告在struct裡則不能realloc Data,可能原因:struct沒有被重新分配記憶體大小
} TypeOfslave_SPI_PacDe;

TypeOfslave_SPI_PacDe slave_SPI_PacDe_str;


void slave_SPI_swap_ini(TypeOfslave_SPI_swap* str_p,TypeOfBuffer* OutBuff_p,TypeOfBuffer* InBuff_p){
	str_p->status=receiving;
	str_p->InBUFF_p=InBuff_p;
	str_p->OutBUFF_p=OutBuff_p;
	str_p->InBUFF_p->PUTindex=0;
	str_p->InBUFF_p->GETindex=0;
	str_p->OutBUFF_p->PUTindex=0;
	str_p->OutBUFF_p->GETindex=0;
}
//通訊封包處理及回應產生器初始函式
void slave_SPI_PacDe_ini(TypeOfslave_SPI_PacDe* str_p,TypeOfBuffer* OutBuff_p,TypeOfBuffer* InBuff_p){
	str_p->status=STATUS_HEADER;
	str_p->Data_p = NULL;
	str_p->OutBUFF_p=OutBuff_p;
	str_p->InBUFF_p=InBuff_p;
}
//SPI通訊處理器執行函式
void slave_SPI_swap_step(void){
	//宣告定義並初始化資料結構指標，以備後續使用。
	TypeOfslave_SPI_swap* str_p=&slave_SPI_swap_str;
	if((str_p->InBUFF_p->PUTindex+1)%MAXBUFFBYTES != str_p->InBUFF_p->GETindex){ // avoid data crush
		str_p-> InBUFF_p->data[ str_p->InBUFF_p->PUTindex ] = SPDR;
		str_p-> InBUFF_p->PUTindex = (str_p->InBUFF_p->PUTindex+1)%MAXBUFFBYTES;
	}
	if(str_p->OutBUFF_p->GETindex != str_p->OutBUFF_p->PUTindex){
		SPDR = str_p->OutBUFF_p->data[ str_p->OutBUFF_p->GETindex ];
		str_p->OutBUFF_p-> GETindex = (str_p->OutBUFF_p->GETindex+1)%MAXBUFFBYTES;
	}
	//R b ver.
	/*
	switch(str_p->status){
		case receiving :
			str_p-> InBUFF_p->data[ str_p->InBUFF_p->PUTindex ] = SPDR;
			str_p-> InBUFF_p->PUTindex = (str_p->InBUFF_p->PUTindex+1)%MAXBUFFBYTES;
			break;
		case transmitting :
			SPDR = str_p->OutBUFF_p->data[ str_p->OutBUFF_p->GETindex ];
			str_p->OutBUFF_p-> GETindex = (str_p->OutBUFF_p->GETindex+1)%MAXBUFFBYTES;
			break;
	}
	 */
}

//SPI通訊處理器狀態切換函式
//未使用
char slave_SPI_swap_ss(TypeOfslave_SPI_swap* str_p){
	char res=0;
	switch(str_p->status){
		case receiving:
			str_p->status= transmitting;
			res=0;
			break;
		case transmitting:
			str_p->status= receiving;
			res=1;
			break; //1 for error message
	}
	return res;
}

//通訊封包解包執行函式
char slave_SPI_PacDe_step(TypeOfslave_SPI_PacDe* str_p){
	//輸入緩衝寫入指標小於等於讀出指標時不執行
	TypeOfslave_SPI_swap* swapstr_p = &slave_SPI_swap_str;
	static char BytesCount = 0;
	static char check_sum = 0;
	char result =0;
	char rcheck_sum = 0;
	char datemp=0;

	if(str_p->InBUFF_p->PUTindex == str_p->InBUFF_p->GETindex ) return 0;
	switch(str_p->status){
		case STATUS_HEADER :
			BytesCount=0;
			check_sum=0;
			result=0;
			if(str_p->InBUFF_p->data[ str_p->InBUFF_p->GETindex ] == HEADER){
				str_p->status = STATUS_CALLTYPE;
				//printf("get header\n");
			}
			str_p->InBUFF_p->GETindex = (str_p->InBUFF_p->GETindex + 1)%MAXBUFFBYTES;
			break;
		case STATUS_CALLTYPE :
			str_p->CallType = str_p->InBUFF_p->data[str_p->InBUFF_p->GETindex];
			//printf("CallType  %d\n", str_p->CallType);
			check_sum = check_sum+str_p->CallType;
			str_p->InBUFF_p->GETindex = (str_p->InBUFF_p->GETindex + 1)%MAXBUFFBYTES;
			str_p->status = STATUS_LSBYTE;
			break;
		case STATUS_LSBYTE:
			str_p->LSByte = str_p->InBUFF_p->data[str_p->InBUFF_p->GETindex];
			//printf("LSByte  %d\n", str_p->LSByte);
			check_sum = check_sum+str_p->LSByte;
			str_p->InBUFF_p->GETindex = (str_p->InBUFF_p->GETindex + 1)%MAXBUFFBYTES;
			switch(str_p->CallType){
				case CALL_TYPE_SET:
				case CALL_TYPE_FPT:
				case CALL_TYPE_FGT:
					str_p->status = STATUS_MASK;
					break;
				case CALL_TYPE_PUT:
				case CALL_TYPE_GET:
					str_p->status = STATUS_BYTES;
					break;
			}
			break;
		case STATUS_BYTES:
			str_p->Bytes = str_p->InBUFF_p->data[str_p->InBUFF_p->GETindex];
			//printf("Bytes  %d\n", str_p->Bytes);
			check_sum = check_sum+str_p->Bytes;

			str_p->Data_p = realloc(str_p->Data_p, str_p->Bytes*sizeof(char));

			str_p->InBUFF_p->GETindex = (str_p->InBUFF_p->GETindex + 1)%MAXBUFFBYTES;
			switch(str_p->CallType){
				case CALL_TYPE_PUT:
					str_p->status = STATUS_DATA;
					break;
				case CALL_TYPE_GET:
					str_p->status = STATUS_CHECKSUM;
					break;
			}
			break;
		case STATUS_MASK:
			str_p->Mask = str_p->InBUFF_p->data[str_p->InBUFF_p->GETindex];
			//printf("Mask  %d\n", str_p->Mask);
			check_sum = check_sum+str_p->Mask;
			str_p->InBUFF_p->GETindex = (str_p->InBUFF_p->GETindex + 1)%MAXBUFFBYTES;
			str_p->status =STATUS_SHIFT;
			break;
		case STATUS_SHIFT:
			str_p->Shift = str_p->InBUFF_p->data[str_p->InBUFF_p->GETindex];
			//printf("Shift  %d\n", str_p->Shift);
			check_sum = check_sum+str_p->Shift;
			str_p->InBUFF_p->GETindex = (str_p->InBUFF_p->GETindex + 1)%MAXBUFFBYTES;
			switch(str_p->CallType){
				case CALL_TYPE_SET:
				case CALL_TYPE_FPT:
					str_p->status = STATUS_DATA;
					break;
				case CALL_TYPE_FGT:
					str_p->status = STATUS_CHECKSUM;
					break;
			}
			break;
		case STATUS_DATA:
			datemp=str_p->InBUFF_p->data[str_p->InBUFF_p->GETindex];
			*((char*)str_p->Data_p + BytesCount) = datemp-BytesCount;
			datemp=*((char*)str_p->Data_p + BytesCount);
			check_sum = check_sum+datemp+BytesCount;
			str_p->InBUFF_p->GETindex = (str_p->InBUFF_p->GETindex + 1)%MAXBUFFBYTES;
			if(str_p->CallType == CALL_TYPE_PUT ){
				BytesCount = BytesCount+1;
				if(BytesCount == str_p->Bytes){
					str_p->status = STATUS_CHECKSUM;
				}
			}
			else {str_p->status = STATUS_CHECKSUM;}
			break;
		case STATUS_CHECKSUM:
			str_p->CheckSum = str_p->InBUFF_p->data[str_p->InBUFF_p->GETindex];
			str_p->InBUFF_p->GETindex = (str_p->InBUFF_p->GETindex + 1)%MAXBUFFBYTES;
			if(check_sum == str_p->CheckSum){
				switch(str_p->CallType){
					case CALL_TYPE_SET:
						datemp=*((char*)str_p->Data_p);
						result=slave_set(str_p->LSByte , str_p->Mask , str_p->Shift , datemp);
						break;
					case CALL_TYPE_PUT:
						result=slave_put(str_p->LSByte ,str_p->Bytes , (char*)str_p->Data_p);
						break;
					case CALL_TYPE_GET:
						result=slave_get(str_p->LSByte ,str_p->Bytes , (char*)str_p->Data_p);
						break;
					case CALL_TYPE_FPT:
						datemp=*((char*)str_p->Data_p);
						result=slave_fpt(str_p->LSByte , str_p->Mask , str_p->Shift , datemp);
						break;
					case CALL_TYPE_FGT:
						result=slave_fgt(str_p->LSByte , str_p->Mask , str_p->Shift , (char*)str_p->Data_p);
						break;
				}
			}
			else{ //check_sum error
			 	result = 1;
			}
			str_p->OutBUFF_p->data[str_p->OutBUFF_p->PUTindex] = REHEADER;
			str_p->OutBUFF_p->PUTindex = (str_p->OutBUFF_p->PUTindex + 1)%MAXBUFFBYTES;
			str_p->OutBUFF_p->data[str_p->OutBUFF_p->PUTindex] = result;
			str_p->OutBUFF_p->PUTindex = (str_p->OutBUFF_p->PUTindex + 1)%MAXBUFFBYTES;
			rcheck_sum=rcheck_sum+result;
			switch(str_p->CallType){
					case CALL_TYPE_SET:
						str_p->OutBUFF_p->data[str_p->OutBUFF_p->PUTindex] = rcheck_sum;
						str_p->OutBUFF_p->PUTindex = (str_p->OutBUFF_p->PUTindex + 1)%MAXBUFFBYTES;
						break;
					case CALL_TYPE_PUT:
						str_p->OutBUFF_p->data[str_p->OutBUFF_p->PUTindex] = rcheck_sum;
						str_p->OutBUFF_p->PUTindex = (str_p->OutBUFF_p->PUTindex + 1)%MAXBUFFBYTES;
						break;
					case CALL_TYPE_GET:
						if(result == 0){
							for(BytesCount = 0 ; BytesCount < str_p->Bytes ; BytesCount++){
								str_p->OutBUFF_p->data[str_p->OutBUFF_p->PUTindex] = *((char*)str_p->Data_p+BytesCount)+BytesCount;
								str_p->OutBUFF_p->PUTindex = (str_p->OutBUFF_p->PUTindex + 1)%MAXBUFFBYTES;
								rcheck_sum = rcheck_sum+*((char*)str_p->Data_p+BytesCount)+BytesCount;
							}
						}
						str_p->OutBUFF_p->data[str_p->OutBUFF_p->PUTindex] = rcheck_sum;
						str_p->OutBUFF_p->PUTindex = (str_p->OutBUFF_p->PUTindex + 1)%MAXBUFFBYTES;
						break;
					case CALL_TYPE_FPT:
						str_p->OutBUFF_p->data[str_p->OutBUFF_p->PUTindex] = rcheck_sum;
						str_p->OutBUFF_p->PUTindex = (str_p->OutBUFF_p->PUTindex + 1)%MAXBUFFBYTES;
						break;
					case CALL_TYPE_FGT:
						if(result == 0){
							str_p->OutBUFF_p->data[str_p->OutBUFF_p->PUTindex] = *((char*)str_p->Data_p+BytesCount);
							str_p->OutBUFF_p->PUTindex = (str_p->OutBUFF_p->PUTindex + 1)%MAXBUFFBYTES;
							rcheck_sum = rcheck_sum+*((char*)str_p->Data_p+BytesCount);
						}
						str_p->OutBUFF_p->data[str_p->OutBUFF_p->PUTindex] = rcheck_sum;
						str_p->OutBUFF_p->PUTindex = (str_p->OutBUFF_p->PUTindex + 1)%MAXBUFFBYTES;
						break;
			}
			rcheck_sum=0;
			str_p->status = STATUS_HEADER;
			//slave_SPI_swap_ss(swapstr_p);
			break;
	}
	return 0;
}

//測試用slave被代CALL函式----------------------------------------------------

char slave_set(char LSByte,char Mask,char Shift,char Data){
	if ( LSByte < 200 || LSByte > 201 ) { return 1; }
	switch(LSByte){
		case 200:
			TestData[0]=(Data<<Shift)&Mask;
			break;
		case 201:
			TestData[1]=(Data<<Shift)&Mask;
			break;
	}
	return 0;
}

char slave_put(char LSByte,char Bytes , void*  Data_p){
	if ( LSByte < 0 || LSByte > 1 ) { return 1; }
	if ( Bytes < 1 || Bytes > 2 ) { return 2; }
	char data=0;
	char* data_p=&data;
	switch(LSByte){
		case 0:
			data_p=&TestData[0];
			break;
		case 1:
			data_p=&TestData[1];
			break;
	}
	for(int i=0;i<Bytes;i++){
		*(data_p+i)=*((char*)Data_p+i);
	}
	return 0;
}

char slave_get(char LSByte,char Bytes,void*  Data_p){
	if ( LSByte < 100 || LSByte > 101 ) { return 1; }
	if ( Bytes < 1 || Bytes > 2 ) { return 2; }
	char data=0;
	char* data_p=&data;
	switch(LSByte){
		case 100:
			data_p=&TestData[0];
			break;
		case 101:
			data_p=&TestData[1];
			break;
	}
	for(int i=0;i<Bytes;i++){
		*((char*)Data_p+i)=*(data_p+i);
	}
	return 0;
}

char slave_fpt(char LSByte,char Mask,char Shift,char Data){
	if ( LSByte < 200 || LSByte > 201 ) { return 1; }
	char data=0;
	char* data_p=&data;
	switch(LSByte){
		case 200:
			data_p=&TestData[0];
			break;
		case 201:
			data_p=&TestData[1];
			break;
	}
	*(data_p)=(Data<<Shift)&Mask;
	return 0;
}

char slave_fgt(char LSByte,char Mask,char Shift,char *Data){
	if ( LSByte < 100 || LSByte > 101 ) { return 1; }
	char data=0;
	char* data_p=&data;
	switch(LSByte){
		case 100:
			data_p=&TestData[0];
			break;
		case 101:
			data_p=&TestData[1];
			break;
	}
	*Data=(*(data_p)&Mask)>>Shift;
	return 0;
}

void ASA_slave_ini(){
    sei();
    DDRB=(0<<DD_SCK)|(0<<DD_MOSI)|(0<<DD_SS)|(1<<DD_MISO);
	SPCR=(1<<SPE)|(0<<MSTR)|(1<<SPIE);
}

int main(void){
	ASA_M128_set();
	ASA_slave_ini();
	TypeOfslave_SPI_swap* str_p=&slave_SPI_swap_str;
	slave_SPI_swap_ini(str_p,&Res_Buff_str,&Com_Buff_str);
	slave_SPI_PacDe_ini(&slave_SPI_PacDe_str,&Res_Buff_str,&Com_Buff_str);
	TestData[0]=87;
	TestData[1]=78;

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

