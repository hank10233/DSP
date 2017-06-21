#ifndef __ASA_TWI_H__
#define __ASA_TWI_H__

#include "..\LLL\ASA_Lib.h"
#include "..\LLL\ASA_DIO.h"


//Status codes for master start
#define TWI_Start 0x08
#define TWI_REP_START		0x10
// Status codes for master transmitter mode
#define TWI_MT_SLA_ACK		0x18
#define TWI_MT_SLA_NACK		0x20
#define TWI_MT_DATA_ACK		0x28
#define TWI_MT_DATA_NACK	0x30
#define TWI_MT_ARB_LOST		0x38
// Status codes for master receiver mode
#define TWI_MR_SLA_ACK		0x40
#define TWI_MR_SLA_NACK		0x48
#define TWI_MR_DATA_ACK		0x50
#define TWI_MR_DATA_NACK	0x58
#define TWI_MR_ARB_LOST	    0x38


char M128_TWI_set( char LSByte, char Mask,  char shift,  char Data);
char M128_TWI_put( char LSByte, char Bytes, char* Data_p);
char M128_TWI_get( char LSByte, char Bytes, char* Data_p);
char M128_TWI_fpt( char LSByte, char Mask,  char shift,  char Data);
char M128_TWI_fgt( char LSByte, char Mask,  char shift,  char* Data_p);
void TWI_movement_Stop(void);
char TWI_movement_Start(void);
char TWI_movement_Restart(void);
char TWI_movement_MSLA_W(void);
char TWI_movement_MSLA_R(void);
char TWI_movement_MDataTransmit(void);
char TWI_movement_MDataReceive_nLast(void);
char TWI_movement_MDataReceive_Last(void);
char M128_TWI_trm(char OneReg, char SLA, char RegAdd, char Bytes, void *Data_p);
char M128_TWI_rec(char OneReg, char SLA, char RegAdd, char Bytes, void *Data_p);


#endif
