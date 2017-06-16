#ifndef __ASA_DIO_h__
#define __ASA_DIO_h__

#include <avr/io.h>
#include "bit_operator.h"
#include "ASA_general.h"

char M128_DIO_set(char LSByte, char Mask, char shift, char Data);
char M128_DIO_fpt(char LSByte, char Mask, char shift, char Data);
char M128_DIO_fgt(char LSByte, char Mask, char shift, char *Data);
char M128_ASAID_SET(char ASAID);

#endif
