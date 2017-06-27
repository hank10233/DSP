#include "..\LLL\ASA_Lib.h"
#include "..\LLL\ASA_general.h"
#include "..\LLL\bit_operator.h"
#include <stdlib.h>

#define twi_slave_address 0b10110000
#define MAXBUFFBYTES 32
#define HEADER 0x77
#define REHEADER 0x79
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
//Status codes for TWI
#define TW_BUS_ERROR 0x00
#define TW_BUS_INI 0xf8
// Status codes for slave transmitter mode
#define TWI_ST_SLA_ACK		0xA8
#define TWI_ST_DATA_ACK		0xB8
#define TWI_ST_DATA_NACK	0xC0
#define TWI_ST_LAST_DATA	0xC8
// Status codes for slave receiver mode
#define TWI_SR_SLA_ACK		0x60
#define TWI_SR_DATA_ACK		0x80
#define TWI_SR_DATA_NACK	0x88
#define TWI_SR_STOP			0xA0

#define TW_STATUS (TWSR & 0xf8)

//緩衝區資料結構型態
typedef struct {
char PUTindex;
char GETindex;
char data[MAXBUFFBYTES];
} TypeOfBuffer;

//TWI通訊處理用資料結構
typedef struct {
char status;
TypeOfBuffer* InBUFF_p;
TypeOfBuffer* OutBUFF_p;
} TypeOfslave_TWI_swap;

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
} TypeOfslave_TWI_PacDe;

TypeOfBuffer Com_Buff_str;
TypeOfBuffer Res_Buff_str;
TypeOfslave_TWI_swap slave_TWI_swap_str;
TypeOfslave_TWI_PacDe slave_TWI_PacDe_str;


void slave_TWI_ini(void);
void slave_TWI_swap_ini(TypeOfslave_TWI_swap* str_p,TypeOfBuffer* OutBuff_p,TypeOfBuffer* InBuff_p);
void slave_TWI_PacDe_ini(TypeOfslave_TWI_PacDe* str_p,TypeOfBuffer* OutBuff_p,TypeOfBuffer* InBuff_p);
void slave_TWI_swap_step(void);
char slave_TWI_PacDe_step(TypeOfslave_TWI_PacDe* str_p);
void slave_TWI_swap_ss(TypeOfslave_TWI_swap* str_p);

char slave_set(char LSByte,char Mask,char Shift,char Data);
char slave_put(char LSByte,char Bytes , void*  Data_p);
char slave_get(char LSByte,char Bytes,void*  Data_p);
char slave_fpt(char LSByte,char Mask,char Shift,char Data);
char slave_fgt(char LSByte,char Mask,char Shift,char *Data);

