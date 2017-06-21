#include "..\LLL\ASA_Lib.h"
#include "..\LLL\ASA_general.h"
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


//緩衝區資料結構型態
typedef struct {
char PUTindex;
char GETindex;
char data[MAXBUFFBYTES];
} TypeOfBuffer;

//SPI通訊處理用資料結構
typedef struct {
char status;
TypeOfBuffer* InBUFF_p;
TypeOfBuffer* OutBUFF_p;
} TypeOfslave_SPI_swap;

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

TypeOfBuffer Com_Buff_str;
TypeOfBuffer Res_Buff_str;
TypeOfslave_SPI_swap slave_SPI_swap_str;
TypeOfslave_SPI_PacDe slave_SPI_PacDe_str;

void slave_SPI_ini();
void slave_SPI_swap_ini(TypeOfslave_SPI_swap* str_p,TypeOfBuffer* OutBuff_p,TypeOfBuffer* InBuff_p);
void slave_SPI_PacDe_ini(TypeOfslave_SPI_PacDe* str_p,TypeOfBuffer* OutBuff_p,TypeOfBuffer* InBuff_p);
void slave_SPI_swap_step(void);
//char slave_SPI_swap_ss(TypeOfslave_SPI_swap* str_p);   未使用
char slave_SPI_PacDe_step(TypeOfslave_SPI_PacDe* str_p);
