#ifndef __ASA_SPI_H__
#define __ASA_SPI_H__

#include <avr/io.h>
#include "ASA_DIO.h"
#include <util\delay.h>

#define HEADER 0x87
#define REHEADER 0x88
#define ASA_ID 1
#define CALL_TYPE_SET 0
#define CALL_TYPE_PUT 1
#define CALL_TYPE_GET 2
#define CALL_TYPE_FPT 3
#define CALL_TYPE_FGT 4

void Master_ini();
char M128_SPI_swap(char cData);
char M128_SPI_set(char LSByte, char Mask,  char shift,  char Data);
char M128_SPI_put(char NoAdd, char Addr, char Bytes, void *Data_p);
char M128_SPI_get(char NoAdd, char Addr, char Bytes, void *Data_p);
char ASA_SPI_set(char ASAID,char LSBbyte,char Mask,char Shift,char Data);
char ASA_SPI_put(char ASAID,char LSBbyte,char Bytes,void* Data_p);
char ASA_SPI_get(char ASAID,char LSBbyte,char Bytes,void* Data_p);
char ASA_SPI_fpt(char ASAID,char LSBbyte,char Mask,char Shift,char Data);
char ASA_SPI_fgt(char ASAID,char LSBbyte,char Mask,char Shift,char* Data_p);


#endif
