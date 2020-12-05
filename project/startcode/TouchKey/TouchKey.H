
#ifndef __TOUCHKEY_H__
#define __TOUCHKEY_H__
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#define   EN_ADC_INT                    0
#define   DOWM_THRESHOLD_VALUE          1000                      //������ֵ����
#define   UP_THRESHOLD_VALUE            50                        //̧����ֵ
#define   KEY_BUF_LEN                  10                       //ÿ��KEY������Ӧ���˲���������С�����޸���Ч��20
#define   KEY_BUF_LOST                  2                       //�����ǰ��ֱ������ֽ��������޸���Ч�� 5

extern UINT16 KeyBuf[16][KEY_BUF_LEN];

extern void TouchKey_Init( void );
extern UINT16 TouchKeySelect( UINT8 ch,UINT8 cpw );
extern UINT16 Default_TouchKey( UINT8 ch,UINT8 cpw );
#endif
