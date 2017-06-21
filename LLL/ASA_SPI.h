#ifndef __ASA_SPI_H__
#define __ASA_SPI_H__

#include <avr/io.h>
#include "ASA_DIO.h"

void Master_ini();
char M128_SPI_swap(char cData);
char M128_SPI_set(char LSByte, char Mask,  char shift,  char Data);
char M128_SPI_put(char NoAdd, char Addr, char Bytes, void *Data_p);
char M128_SPI_get(char NoAdd, char Addr, char Bytes, void *Data_p);

#endif
