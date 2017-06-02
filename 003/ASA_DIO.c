#include "ASA_DIO.h"

char M128_DIO_set(char LSByte, char Mask, char shift, char Data) {
	volatile uint8_t *DDR;
	if( LSByte<200||LSByte>206 )
		return 1;
	if( Mask<0||Mask>255 ){
		return 1;
	}
	if( shift<0||shift>7 )
		return 1;
	switch(LSByte){
		case 200:
			DDR = &DDRA; break;
		case 201:
			DDR = &DDRB; break;
		case 202:
			DDR = &DDRC; break;
		case 203:
			DDR = &DDRD; break;
		case 204:
			DDR = &DDRE; break;
		case 205:
			DDR = &DDRF; break;
		case 206:
			DDR = &DDRG; break;
		default:
			return 2;
	}
	bits_put(Data, *DDR, Mask, shift);
	return 0;
}

char M128_DIO_fpt(char LSByte, char Mask, char shift, char Data) {
	volatile uint8_t *DDR;
	volatile uint8_t *PORT;
    switch( LSByte ) {
        case 0:
			DDR = &DDRA; PORT = &PORTA; break;
        case 1:
			DDR = &DDRB; PORT = &PORTB; break;
        case 2:
			DDR = &DDRC; PORT = &PORTC; break;
        case 3:
			DDR = &DDRD; PORT = &PORTD; break;
        case 4:
			DDR = &DDRE; PORT = &PORTE; break;
        case 5:
			DDR = &DDRF; PORT = &PORTF; break;
        case 6:
			DDR = &DDRG; PORT = &PORTG; break;
        default:
			return 3;
    }
	if( (~*DDR) & Mask )
		return 1;
	if(  (~Mask) & (Data<<shift) )
		return 2;

    bits_put(Data, *PORT, Mask, shift);
    return 0;
}

char M128_DIO_fgt(char LSByte, char Mask, char shift, char *Data) {
	volatile uint8_t *DDR;
	volatile uint8_t *PIN;
    switch(LSByte) {
        case 100:
			DDR = &DDRA;PIN = &PINA; break;
        case 101:
			DDR = &DDRB;PIN = &PINB; break;
        case 102:
			DDR = &DDRC;PIN = &PINC; break;
        case 103:
			DDR = &DDRD;PIN = &PIND; break;
        case 104:
			DDR = &DDRE;PIN = &PINE; break;
        case 105:
			DDR = &DDRF;PIN = &PINF; break;
        case 106:
			DDR = &DDRG;PIN = &PING; break;
        default:
			return 3;
    }
    if( (*DDR) & Mask )
		return 1;
	*Data=bits_get(*PIN, Mask, shift);
    return 0;
}
