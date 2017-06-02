#include <avr\io.h>
#include <avr\interrupt.h>
#include <stdio.h>
#include <util\delay.h>

#define DD_SS   PB2
#define DD_MOSI PB3
#define DD_MISO PB4
#define DD_SCK  PB5

void spi_ini (void);
char data=0;

int main (void) {
	spi_ini();
	while(1) {
	}
}

void spi_ini (void) {
	sei();
	SPCR=(1<<SPIE)|(1<<SPE)|(0<<MSTR);						//設定為slave
	DDRB=(0<<DD_SS)|(0<<DD_SCK)|(0<<DD_MOSI)|(1<<DD_MISO);  //SCK輸入
}

ISR(SPI_STC_vect){
	data=SPDR;
	data++;
	SPDR=data;
}
