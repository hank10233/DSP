#include "ASA_Lib.h"
#include "ASA_MAX7219.h"

#define MAX7219_ADDR_DECODE       0x09
#define MAX7219_ADDR_INTENSITY    0x0A
#define MAX7219_ADDR_SCAN_LIMIT   0x0B
#define MAX7219_ADDR_POWER        0x0C
#define MAX7219_ADDR_TEST         0x0F
#define MAX7219_ADDR_NOOP         0x00


int main() {
    ASA_M128_set();
    printf("start!-----------------------------\n");
    char ASA_ID = 1;
    char check,i;
    char data,Addr,LSByte;
    int input;

   char matrix[8]={
    0b11111111,
    0b01000000,
    0b00100000,
    0b00010000,
    0b00001000,
    0b00000100,
    0b00000010,
    0b11111111
    };
/*


 */
    ASA_MAX7219_ini();
    M128_DIO_fpt(ADDR_PORT_num,ADDR_PORT_msk,ADDR_PORT_sht,ASA_ID);
    _delay_ms(250);
    printf("%d", bit_get(SPCR,CPOL));
    data=1,LSByte=203;ASA_MAX7219_set(ASA_ID,LSByte,1,0xFF,0,&data); // 關閉 SHOTDOWN模式
    data=0,LSByte=204;ASA_MAX7219_set(ASA_ID,LSByte,1,0xFF,0,&data); // 關閉 DISPLAY_TEST MODE
    data=7,LSByte=202;ASA_MAX7219_set(ASA_ID,LSByte,1,0xFF,0,&data); // 顯示7位數
    data=0,LSByte=201;ASA_MAX7219_set(ASA_ID,LSByte,1,0xFF,0,&data); // 亮度0(最低)
    _delay_ms(200);
    for(i=0;i<8;i++){
        ASA_MAX7219_put(ASA_ID,i+1,1,&matrix[i]);
        _delay_ms(10);
    }
/*
while(1){
check=M128_DIO_set(200,0xff,0,0x0f);
            printf("%d\n",check);
            check=M128_DIO_fpt(0,0x03,0,0x03);
            printf("%d\n",check);
            check=M128_DIO_fgt(100,0xf0,1,&data);
            printf("%d\n",check);

            //scanf("%d", &input);
            //data = input;
            //ASA_MAX7219_put(ASA_ID,,1,&data);
            //M128_SPI_put(0,3,1,&data);
            M128_DIO_fpt(CS_PORT_NUM, CS_PORT_MSK, CS_PORT_SHT, 1);
            M128_SPI_swap(MAX7219_ADDR_TEST);
            M128_SPI_swap(data);
            M128_SPI_put(0,MAX7219_ADDR_TEST, 1,&data);
            M128_DIO_fpt(CS_PORT_NUM, CS_PORT_MSK, CS_PORT_SHT, 0);
}
 */

    return 0;
}
