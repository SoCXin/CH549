/********************************** (C) COPYRIGHT *******************************
 * File Name          : TouchKey.C
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2019/07/22
 * Description        : CH547 TouchKey触摸按键
 *                      CH0~CH15 分别对应引脚 P1.0~P1.7  P0.0~P0.7
 *******************************************************************************/
#include ".\TouchKey\TouchKey.H"
#include <stdlib.h>
#pragma NOAREGS

UINT16 IntCurValue = 0;  //中断采集到的当前值
UINT16 KeyBuf[16][KEY_BUF_LEN];
UINT16 PowerValue[16];
UINT16 Keyvalue[16];
UINT8 keyChannel     = 8;
UINT8C CPW_Table[16] = {30, 30, 30, 30, 30, 30, 30, 30,  //与板间电容有关的参数，分别对应每个按键
                        30, 30, 30, 30, 30, 30, 30, 30};
UINT8 keyTime[16]    = {0};
UINT16 keyData       = 0;
volatile _TKS_FLAGA16_type keyTrg[2];
UINT16 k_count[2];

#define RESTAIN_TIMES 200  // 200 × 10ms = 2s
/*******************************************************************************
 * Function Name  : TouchKey_Init
 * Description    : 触摸按键初始化
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void TouchKey_Init(void)
{
    // Touch采样通道设置为高阻输入
    P0_MOD_OC &= 0xF0;  // P00 P01 P02 P03高阻输入
    P0_DIR_PU &= 0xF0;
    ADC_CFG |= (bADC_EN | bADC_AIN_EN);             //开启ADC模块电源,选择外部通道
    ADC_CFG  = ADC_CFG & ~(bADC_CLK0 | bADC_CLK1);  //选择ADC参考时钟
    ADC_CHAN = (3 << 4);                            //默认选择外部通道0
    ADC_CTRL = bADC_IF;                             //清除ADC转换完成标志，写1清零

    touchKeyFirstValue();
#if EN_ADC_INT
    SCON2 &= ~bU2IE;  //和UART2中断地址共用，故中断需2选1
    IE_ADC = 1;       //开启ADC中断使能
    EA     = 1;       //开启总中断使能
#endif
    TouchKeychannelSelect(CPW_Table[keyChannel]);
}

void touchKeyFirstValue(void)
{
    UINT8 ch;
    /* 获取按键初值 */
    for (ch = 8; ch != 12; ch++)
    {
        PowerValue[ch] = Default_TouchKey(ch, CPW_Table[ch]);
        printf(" Y%d ", PowerValue[ch]);
    }
    printf("\n");
}
/*******************************************************************************
* Function Name  : UpDataBuf
* Description    : 去尖峰平均值滑动滤波
                   更新窗口数组,首移出，把newdat插入数组尾,数组大小 KEY_BUF_LEN
*                  排序，去值，取平均返回
* Input          : buf,newdat
* Output         : None
* Return         : None
*******************************************************************************/
UINT16X Temp_Buf[KEY_BUF_LEN];  //专用缓冲区
UINT16 Buf_UpData_Filter(UINT16 *buf, UINT16 newdat)
{
    UINT8 i, j, k;
    UINT16 temp;
    UINT32 sum;

    //更新队列
    for (i = 1; i != KEY_BUF_LEN; i++)
    {
        buf[i - 1] = buf[i];
    }
    buf[i - 1] = newdat;

    //排序
    memcpy(Temp_Buf, buf, sizeof(Temp_Buf));
    k = KEY_BUF_LEN - 1;
    for (i = 0; i != k; i++)
    {
        for (j = i + 1; j != KEY_BUF_LEN; j++)
        {
            if (Temp_Buf[i] < Temp_Buf[j])
            {
                temp        = Temp_Buf[i];
                Temp_Buf[i] = Temp_Buf[j];
                Temp_Buf[j] = temp;
            }
        }
    }

    //丢值，取平均
    sum = 0;
    k   = KEY_BUF_LEN - KEY_BUF_LOST;
    for (i = KEY_BUF_LOST; i != k; i++)
    {
        sum += Temp_Buf[i];
    }
    return (sum / (KEY_BUF_LEN - 2 * KEY_BUF_LOST));
}

/*******************************************************************************
* Function Name  : TouchKey_Init
* Description    : 触摸按键初始化
* Input          : 通道号选择ch:0~15,分别对应P10~P17、P00~P07
                   充电脉冲宽度 cpw:0~127
                   cpw由 外部触摸按键电容、VDD电压、主频三者决定。
                   计算公式：count=(Ckey+Cint)*0.7VDD/ITKEY/(2/Fsys)
                   TKEY_CTRL=count > 127 ? 127 : count （其中Cint为15pF,ITKEY为50u）
                   简化公式：cpw = （Ckey+15）*0.35*VDD*Fsys/50
                   cpw = cpw>127?127:cpw
* Output         : None
* Return         : 返回触摸检测电压
*******************************************************************************/
UINT16 TouchKeySelect(UINT8 ch, UINT8 cpw)
{
    ADC_CHAN = ADC_CHAN & (~MASK_ADC_CHAN) | ch;  //外部通道选择
    //电容较大时可以先设置IO低，然后恢复浮空输入实现手工放电，≤0.2us
    TKEY_CTRL = cpw;  //充电脉冲宽度配置，仅低7位有效（同时清除bADC_IF，启动一次TouchKey）
    while (ADC_CTRL & bTKEY_ACT)
        ;
    IntCurValue = (ADC_DAT & 0xFFF);
    TKEY_CTRL   = 0;
    IntCurValue = Buf_UpData_Filter(&KeyBuf[ch][0], IntCurValue);
    return (IntCurValue);
}

UINT16 Default_TouchKey(UINT8 ch, UINT8 cpw)
{
    ADC_CHAN = ADC_CHAN & (~MASK_ADC_CHAN) | ch;  //外部通道选择
    //电容较大时可以先设置IO低，然后恢复浮空输入实现手工放电，≤0.2us
    TKEY_CTRL = cpw;  //充电脉冲宽度配置，仅低7位有效（同时清除bADC_IF，启动一次TouchKey）
    while (ADC_CTRL & bTKEY_ACT)
        ;
    IntCurValue = (ADC_DAT & 0xFFF);
    TKEY_CTRL   = 0;
    return (IntCurValue);
}

void touchKeyGet(void)
{
    if (ADC_CTRL & bADC_IF)
    {
        ADC_CTRL             = bADC_IF;  //清除ADC转换完成中断标志
        IntCurValue          = (ADC_DAT & 0xFFF);
        Keyvalue[keyChannel] = Buf_UpData_Filter(&KeyBuf[keyChannel][0], IntCurValue);
        P0_DIR_PU |= 1 << (keyChannel - 8);
        P0 &= ~(1 << (keyChannel - 8));
        keyChannel++;
        if (keyChannel == 12)
            keyChannel = 8;
        TouchKeychannelSelect(CPW_Table[keyChannel]);
    }
}

void getKeyBitMap(void)
{
    UINT8 i;
    UINT16 keyState = 0;
    UINT16 err;  //触摸模拟变化差值
    for (i = 8; i < 12; i++)
    {
        err = abs(Keyvalue[i] - PowerValue[i]);
        if (err > DOWM_THRESHOLD_VALUE)
            keyState |= (1 << i);
        if (err < UP_THRESHOLD_VALUE)
            keyState &= ~(1 << i);
    }

    keyTrg[0].word = keyState & (keyState ^ k_count[0]);
    k_count[0]     = keyState;

    if (keyTrg[0].word)
    {
        beepCount++;
    }
    keyState = 0;
    for (i = 8; i < 12; i++)
    {
        if (k_count[0] & (1 << i))
        {
            if (keyTime[i] < RESTAIN_TIMES)
                keyTime[i]++;
            else
                keyState |= (1 << i);
        }
        else
        {
            // if (keyTime[i] > 0)  // short press
            //     Key_Up_Trg |= (1 << i);
            // else
            //     Key_Up_Trg &= (~(1 << i));

            keyTime[i] = 0;
            keyState &= (~(1 << i));
        }
    }
    keyTrg[1].word = keyState & (keyState ^ k_count[1]);
    k_count[1]     = keyState;
    if (keyTrg[1].word)
    {
        beepCount++;
    }
}

/*******************************************************************************
* Function Name  : TouchKeychannelSelect
* Description    : 触摸按键初始化
* Input          : 通道号选择ch:0~15,分别对应P10~P17、P00~P07
                   充电脉冲宽度 cpw:0~127
                   cpw由 外部触摸按键电容、VDD电压、主频三者决定。
                   计算公式：count=(Ckey+Cint)*0.7VDD/ITKEY/(2/Fsys)
                   TKEY_CTRL=count > 127 ? 127 : count （其中Cint为15pF,ITKEY为50u）
                   简化公式：cpw = （Ckey+15）*0.35*VDD*Fsys/50
                   cpw = cpw>127?127:cpw
* Output         : None
* Return         : 返回触摸检测电压
*******************************************************************************/
void TouchKeychannelSelect(UINT8 cpw)
{
    P0_DIR_PU &= ~(1 << (keyChannel - 8));
    ADC_CHAN = ADC_CHAN & (~MASK_ADC_CHAN) | keyChannel;  //外部通道选择
    //电容较大时可以先设置IO低，然后恢复浮空输入实现手工放电，≤0.2us
    TKEY_CTRL = cpw;  //充电脉冲宽度配置，仅低7位有效（同时清除bADC_IF，启动一次TouchKey）
}

#if EN_ADC_INT
void touchKeyInterrupt(void) interrupt INT_NO_ADC
{
    if (ADC_CTRL & bADC_IF)
    {
        ADC_CTRL             = bADC_IF;  //清除ADC转换完成中断标志
        IntCurValue          = (ADC_DAT & 0xFFF);
        Keyvalue[keyChannel] = Buf_UpData_Filter(&KeyBuf[keyChannel][0], IntCurValue);
        P0_DIR_PU |= 1 << (keyChannel - 8);
        P0 &= ~(1 << (keyChannel - 8));
        keyChannel++;
        if (keyChannel == 12)
            keyChannel = 8;
        TouchKeychannelSelect(CPW_Table[keyChannel]);
    }
}
#endif