/********************************** (C) COPYRIGHT *******************************
* File Name          : Timer.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/21
* Description        : CH549 Time ��ʼ������ʱ������������ֵ��T2��׽���ܿ���������
                       ��ʱ���жϺ���
*******************************************************************************/
#include ".\Timer\Timer.H"
#pragma  NOAREGS
#ifdef  T2_CAP
UINT16 Cap2[2] = {0};
UINT16 Cap1[2] = {0};
UINT16 Cap0[2] = {0};
#endif
/*******************************************************************************
* Function Name  : mTimer_x_ModInit(UINT8 x ,UINT8 mode)
* Description    : CH549��ʱ������xģʽ����
* Input          : UINT8 mode,Timerģʽѡ��
                   0��ģʽ0��13λ��ʱ����TLn�ĸ�3λ��Ч
                   1��ģʽ1��16λ��ʱ��
                   2��ģʽ2��8λ�Զ���װ��ʱ��
                   3��ģʽ3������8λ��ʱ��  Timer0
                   3��ģʽ3��Timer1ֹͣ
* Output         : None
* Return         : �ɹ�  SUCCESS
                   ʧ��  FAIL
*******************************************************************************/
UINT8 mTimer_x_ModInit(UINT8 x ,UINT8 mode)
{
    if(x == 0)
    {
        TMOD = TMOD & 0xf0 | mode;
    }
    else if(x == 1)
    {
        TMOD = TMOD & 0x0f | (mode<<4);
    }
    else if(x == 2)
    {
        RCLK = 0;    //16λ�Զ����ض�ʱ��
        TCLK = 0;
        CP_RL2 = 0;
    }
    else
    {
        return FAIL;
    }
    return SUCCESS;
}
/*******************************************************************************
* Function Name  : mTimer_x_SetData(UINT8 x,UINT16 dat)
* Description    : CH549Timer0 TH0��TL0��ֵ
* Input          : UINT16 dat;��ʱ����ֵ
* Output         : None
* Return         : None
*******************************************************************************/
void mTimer_x_SetData(UINT8 x,UINT16 dat)
{
    UINT16 tmp;
    tmp = 65536 - dat;
    if(x == 0)
    {
        TL0 = tmp & 0xff;
        TH0 = (tmp>>8) & 0xff;
    }
    else if(x == 1)
    {
        TL1 = tmp & 0xff;
        TH1 = (tmp>>8) & 0xff;
    }
    else if(x == 2)
    {
        RCAP2L = TL2 = tmp & 0xff;                                               //16λ�Զ����ض�ʱ��
        RCAP2H = TH2 = (tmp>>8) & 0xff;
    }
}
/*******************************************************************************
* Function Name  : CAP2Init(UINT8 mode)
* Description    : CH549��ʱ������2 T2EX���Ų�׽���ܳ�ʼ����CAP2 P11��
                   UINT8 mode,���ز�׽ģʽѡ��
                   0:T2ex���½��ص���һ���½���
                   1:T2ex�������֮��
                   3:T2ex�������ص���һ��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAP2Init(UINT8 mode)
{
    RCLK = 0;
    TCLK = 0;
    C_T2  = 0;
    EXEN2 = 1;
    CP_RL2 = 1;                                                                //����T2ex�Ĳ�׽����
    T2MOD |= mode << 2;                                                        //���ز�׽ģʽѡ��
}
/*******************************************************************************
* Function Name  : CAP1Init(UINT8 mode)
* Description    : CH549��ʱ������2 T2���Ų�׽���ܳ�ʼ��T2(CAP1 P10)
                   UINT8 mode,���ز�׽ģʽѡ��
                   0:T2ex���½��ص���һ���½���
                   1:T2ex�������֮��
                   3:T2ex�������ص���һ��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAP1Init(UINT8 mode)
{
    RCLK = 0;
    TCLK = 0;
    CP_RL2 = 1;
    C_T2 = 0;
    T2MOD = T2MOD & ~T2OE | (mode << 2) | bT2_CAP1_EN;                         //ʹ��T2���Ų�׽����,���ز�׽ģʽѡ��
}
/*******************************************************************************
* Function Name  : CAP0Init(UINT8 mode)
* Description    : CH549��ʱ������2 CAP0���Ų�׽���ܳ�ʼ��(CAP0 P36)
                   UINT8 mode,���ز�׽ģʽѡ��
                   0:T2ex���½��ص���һ���½���
                   1:T2ex�������֮��
                   3:T2ex�������ص���һ��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAP0Init(UINT8 mode)
{
    RCLK = 0;
    TCLK = 0;
    CP_RL2 = 1;
    C_T2 = 0;
    T2MOD |= mode << 2;                                                        //���ز�׽ģʽѡ��
    T2CON2 = bT2_CAP0_EN;
}
#ifdef T0_INT
/*******************************************************************************
* Function Name  : mTimer0Interrupt()
* Description    : CH549��ʱ������0��ʱ�������жϴ������� 1ms�ж�
*******************************************************************************/
void mTimer0Interrupt( void ) interrupt INT_NO_TMR0 using 1                    //timer0�жϷ������,ʹ�üĴ�����1
{
    mTimer_x_SetData(0,2000);                                                 //ģʽ1 �����¸�TH0��TL0��ֵ
    SCK = ~SCK;                                                               //��Լ1ms
}
#endif
#ifdef T1_INT
/*******************************************************************************
* Function Name  : mTimer1Interrupt()
* Description    : CH549��ʱ������1��ʱ�������жϴ������� 100us�ж�
*******************************************************************************/
void mTimer1Interrupt( void ) interrupt INT_NO_TMR1 using 2                //timer1�жϷ������,ʹ�üĴ�����2
{
    //��ʽ2ʱ��Timer1�Զ���װ
    static UINT16 tmr1 = 0;
    tmr1++;
    if(tmr1 == 2000)                                                       //100us*2000 = 200ms
    {
        tmr1 = 0;
        SCK = ~SCK;
    }
}
#endif
/*******************************************************************************
* Function Name  : mTimer2Interrupt()
* Description    : CH549��ʱ������0��ʱ�������жϴ�������
*******************************************************************************/
void mTimer2Interrupt( void ) interrupt INT_NO_TMR2 using 3                //timer2�жϷ������,ʹ�üĴ�����3
{
#ifdef  T2_INT
    static UINT8 tmr2 = 0;
    if(TF2)
    {
        TF2 = 0;                                                             //��ն�ʱ��2����ж�,���ֶ���
        tmr2++;
        if(tmr2 == 100)                                                      //200msʱ�䵽
        {
            tmr2 = 0;
            SCK = ~SCK;                                                      //P17��ƽָʾ���
        }
    }
#endif

#ifdef  T2_CAP
    if(EXF2)                                                                //T2ex��ƽ�仯�ж��жϱ�־
    {
        Cap2[0] = RCAP2;                                                    //T2EX
        printf("CAP2 %04x\n",Cap2[0]-Cap2[1]);
        Cap2[1] = Cap2[0];
        EXF2 = 0;                                                           //���T2ex��׽�жϱ�־
    }
    if(CAP1F)                                                               //T2��ƽ��׽�жϱ�־
    {
        Cap1[0] = T2CAP1;                                                    //T2;
        printf("CAP1 %04x\n",Cap1[0]-Cap1[1]);
        Cap1[1] = Cap1[0];
        CAP1F = 0;                                                          //���T2��׽�жϱ�־
    }
    if(T2CON2&bT2_CAP0F)
    {
        Cap0[0] = T2CAP0;
        printf("CAP0 %04x\n",Cap0[0]-Cap0[1]);
        Cap0[1] = Cap0[0];
        T2CON2 &= ~bT2_CAP0F;
    }
#endif

}