#include "ASA_Lib.h"
#include "ASA_MAX7219.h"

int main() {
    ASA_M128_set();
    printf("start!-----------------------------\n");
    char ASA_ID = 2;
    char check,i;
    char Data,Addr,LSByte;
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

    ASA_MAX7219_ini();
    Data=1,LSByte=203;ASA_MAX7219_set(ASA_ID,LSByte,0,0xFF,0,&Data); // 關閉 SHOTDOWN模式
    Data=0,LSByte=204;ASA_MAX7219_set(ASA_ID,LSByte,0,0xFF,0,&Data); // 關閉 DISPLAY_TEST MODE
    Data=7,LSByte=202;ASA_MAX7219_set(ASA_ID,LSByte,0,0xFF,0,&Data); // 顯示7位數
    Data=0,LSByte=201;ASA_MAX7219_set(ASA_ID,LSByte,0,0xFF,0,&Data); // 亮度0(最低)
    _delay_ms(200);
    for(i=1;i<=8;i++){
        ASA_MAX7219_put(ASA_ID,i,0,&matrix[i-1]);
        _delay_ms(10);
    }
    while (1) {
        printf("OAO");
    }
    return 0;
}
