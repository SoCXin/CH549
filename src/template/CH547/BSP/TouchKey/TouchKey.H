
#ifndef __TOUCHKEY_H__
#define __TOUCHKEY_H__
#include ".\Public\CH547.H"
#include ".\Public\DEBUG.H"
#define EN_ADC_INT 0
#define DOWM_THRESHOLD_VALUE 400  //按下阈值定义
#define UP_THRESHOLD_VALUE 50     //抬起阈值
#define KEY_BUF_LEN 10            //每个KEY按键对应的滤波缓冲区大小（库修改无效）20
#define KEY_BUF_LOST 2            //排序后前后分别丢弃的字节数（库修改无效） 5

typedef struct
{
    UINT16 b0 : 1;
    UINT16 b1 : 1;
    UINT16 b2 : 1;
    UINT16 b3 : 1;
    UINT16 b4 : 1;
    UINT16 b5 : 1;
    UINT16 b6 : 1;
    UINT16 b7 : 1;
    UINT16 b8 : 1;
    UINT16 b9 : 1;
    UINT16 b10 : 1;
    UINT16 b11 : 1;
    UINT16 b12 : 1;
    UINT16 b13 : 1;
    UINT16 b14 : 1;
    UINT16 b15 : 1;
} _FLAG_16bits;

typedef union
{
    _FLAG_16bits bits;
    UINT16 word;
} _TKS_FLAGA16_type;

extern UINT16 KeyBuf[16][KEY_BUF_LEN];
extern UINT8 keyChannel;
extern UINT8C CPW_Table[16];
extern UINT16 Keyvalue[16];
extern volatile _TKS_FLAGA16_type keyTrg[2];
extern volatile UINT16 k_count[2];

extern UINT8 beepCount;

#define KEY1 keyTrg[0].bits.b11
#define KEY2 keyTrg[0].bits.b10
#define KEY3 keyTrg[0].bits.b9
#define KEY4 keyTrg[0].bits.b8

#define KEY1Restan keyTrg[1].bits.b11
#define KEY2Restan keyTrg[1].bits.b10
#define KEY3Restan keyTrg[1].bits.b9
#define KEY4Restan keyTrg[1].bits.b8

extern void TouchKey_Init(void);
extern UINT16 TouchKeySelect(UINT8 ch, UINT8 cpw);
extern UINT16 Default_TouchKey(UINT8 ch, UINT8 cpw);
void TouchKeychannelSelect(UINT8 cpw);
void getKeyBitMap(void);
void ADCInterruptEnable(void);
void touchKeyFirstValue(void);
void touchKeyGet(void);
#endif
