
#ifndef __TOUCHKEY_H__
#define __TOUCHKEY_H__
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#define   EN_ADC_INT                    0
#define   DOWM_THRESHOLD_VALUE          1000                      //按下阈值定义
#define   UP_THRESHOLD_VALUE            50                        //抬起阈值
#define   KEY_BUF_LEN                  10                       //每个KEY按键对应的滤波缓冲区大小（库修改无效）20
#define   KEY_BUF_LOST                  2                       //排序后前后分别丢弃的字节数（库修改无效） 5

extern UINT16 KeyBuf[16][KEY_BUF_LEN];

extern void TouchKey_Init( void );
extern UINT16 TouchKeySelect( UINT8 ch,UINT8 cpw );
extern UINT16 Default_TouchKey( UINT8 ch,UINT8 cpw );
#endif
