#ifndef __bit_operator_h__
#define __bit_operator_h__

#define bit(m) (1<<(m))
//returns an integer with the m'th bit set to 1
#define bit_set(p,m)   ((p)|=  bit(m))
//p= p | bit(m),sets the mth bit of p to 1
#define bit_clear(p,m) ((p)&=~(bit(m)))
//p= p & ~(bit(m)) sets the mth bit of p to 0
#define bit_check(p,m) ((p)&(bit(m)))
//check the m'th bit is 0 or not 0
#define bit_get(p,m) (((p)&(bit(m))) >>m)
//get the m'th bit is 0 or 1
#define bit_put(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m))
//put c into the m'th bit of p
#define bit_flip(p,m) bit_put((~(bit_get(p,m))),p,m)
//flip the mth bit of p, 1->0, 0->1
#define mask  (bit(1)|bit(2)|bit(3))
//mask is group mask has bit 1,2,3 setted
#define bits_set(dest, mask) ((dest)|=(mask))
//設定目標變數的遮罩區位元為1
#define bits_clear(dest, mask) ((dest)&=(~mask))
//清除目標變數的遮罩區位元為0
#define bits_get(source, mask, shift) (((source) & (mask)) >> shift)
//讀取來源變數之連續遮罩區位元串並向右位移對齊取得遮罩區位元串對應整數值
#define bits_put(source, dest, mask, shift) (dest = (dest&~mask) | ((source <<shift) & mask))
//將資料來源左移到其所佔用旗標群區域後蓋寫到目標變數

#endif
