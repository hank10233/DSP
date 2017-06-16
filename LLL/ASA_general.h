#define ADDR_PORT_num 1
#define ADDR_PORT_msk (1<<5)|(1<<6)|(1<<7)
#define ADDR_PORT_sht 5
// SPI ADDR use PORTB bit 5,6,7 as id 0~7
// use DIO_put to set ADDR

#define DDR_SPI_num 1
#define DD_MISO PB3
#define DD_MOSI PB2
#define DD_SCK  PB1
#define DD_SS   PB0
// SPI use PORTB

#define CS_pin PF4
#define CS_PORT_NUM 5
#define CS_PORT_MSK (1<<CS_pin)
#define CS_PORT_SHT CS_pin
// CS : PF4
