/********************************** (C) COPYRIGHT *******************************
* File Name          : Timer.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/21
* Description        : CH547 Time ��ʼ������ʱ������������ֵ��T2��׽���ܿ���������
                       ��ʱ���жϺ���
*******************************************************************************/
#include ".\Timer\Timer.H"
#pragma NOAREGS
#ifdef T2_CAP2
UINT16 Cap_2[2] = {0};
#endif
#ifdef T0_INT
UINT16 counter1Ms = 0;
bit flag1ms;
bit flag10;
bit flag63;
bit flag250;
bit flag500;
#endif
/*******************************************************************************
* Function Name  : mTimer_x_ModInit(UINT8 x ,UINT8 mode)
* Description    : CH547��ʱ������xģʽ����
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
UINT8 mTimer_x_ModInit(UINT8 x, UINT8 mode)
{
    if (x == 0)
    {
        TMOD = TMOD & 0xf0 | mode;
    }
    else if (x == 1)
    {
        TMOD = TMOD & 0x0f | (mode << 4);
    }
    else if (x == 2)
    {
        RCLK   = 0;  // 16λ�Զ����ض�ʱ��
        TCLK   = 0;
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
 * Description    : CH547Timer0 TH0��TL0��ֵ
 * Input          : UINT16 dat;��ʱ����ֵ
 * Output         : None
 * Return         : None
 *******************************************************************************/
void mTimer_x_SetData(UINT8 x, UINT16 dat)
{
    UINT16 tmp;
    tmp = 65536 - dat;
    if (x == 0)
    {
        TL0 = tmp & 0xff;
        TH0 = (tmp >> 8) & 0xff;
    }
    else if (x == 1)
    {
        TL1 = tmp & 0xff;
        TH1 = (tmp >> 8) & 0xff;
    }
    else if (x == 2)
    {
        RCAP2L = TL2 = tmp & 0xff;  // 16λ�Զ����ض�ʱ��
        RCAP2H = TH2 = (tmp >> 8) & 0xff;
    }
}
/*******************************************************************************
* Function Name  : CAP2Init(UINT8 mode)
* Description    : CH547��ʱ������2 T2EX���Ų�׽���ܳ�ʼ����CAP2 P11��
                   UINT8 mode,���ز�׽ģʽѡ��
                   0:T2ex���½��ص���һ���½���
                   1:T2ex�������֮��
                   3:T2ex�������ص���һ��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef T2_CAP2
void CAP2Init(UINT8 mode)
{
    RCLK = 0;
    TCLK = 0;
    C_T2 = 0;

    // T2MOD &= ~(bT2_CLK | bTMR_CLK);
    T2MOD |= (mode << 2);  //���ز�׽ģʽѡ��
    CP_RL2 = 1;            //����T2ex�Ĳ�׽����
    TR2    = 1;

    EXEN2 = 1;
    ET2   = 1;
    EA    = 1;
}
#endif
#ifdef T0_INT
/*******************************************************
 *
 *
 * */
void timer0Init(void)
{
    printf("T0 Test ...\n");
    PT0 = 1;
    mTimer0Clk12DivFsys();      // T0��ʱ��ʱ������ FREQ_SYS/12
    mTimer_x_ModInit(0, 1);     // T0 ��ʱ��ģʽ���� ģʽ1 16λ��ʱ��
    mTimer_x_SetData(0, 2000);  // T0��ʱ����ֵ 24MHZ 1MS�ж�
    mTimer0RunCTL(1);           // T0��ʱ������
    ET0 = 1;                    // T0��ʱ���жϿ���
    EA  = 1;
}
/*******************************************************************************
 * Function Name  : mTimer0Interrupt()
 * Description    : CH547��ʱ������0��ʱ�������жϴ������� 1ms�ж�
 *******************************************************************************/
void mTimer0Interrupt(void) interrupt INT_NO_TMR0 using 1  // timer0�жϷ������,ʹ�üĴ�����1
{
    mTimer_x_SetData(0, 2000);  //ģʽ1 �����¸�TH0��TL0��ֵ
    SCK = ~SCK;                 //��Լ1ms
    counter1Ms++;
    flag1ms = 1;
    if (counter1Ms % 10 == 0)
        flag10 = 1;
    if (counter1Ms % 63 == 0)
        flag63 = 1;
    if (counter1Ms % 250 == 0)
        flag250 = 1;
    if (counter1Ms % 500 == 0)
        flag500 = 1;
}
#endif
#ifdef T1_INT
/*******************************************************************************
 * Function Name  : mTimer1Interrupt()
 * Description    : CH547��ʱ������1��ʱ�������жϴ������� 100us�ж�
 *******************************************************************************/
void mTimer1Interrupt(void) interrupt INT_NO_TMR1 using 2  // timer1�жϷ������,ʹ�üĴ�����2
{
    //��ʽ2ʱ��Timer1�Զ���װ
    static UINT16 tmr1 = 0;
    tmr1++;
    if (tmr1 == 2000)  // 100us*2000 = 200ms
    {
        tmr1 = 0;
        SCK  = ~SCK;
    }
}
#endif
/*******************************************************************************
 * Function Name  : mTimer2Interrupt()
 * Description    : CH547��ʱ������0��ʱ�������жϴ�������
 *******************************************************************************/
void mTimer2Interrupt(void) interrupt INT_NO_TMR2 using 3  // timer2�жϷ������,ʹ�üĴ�����3
{
#ifdef T2_INT
    static UINT8 tmr2 = 0;
    if (TF2)
    {
        TF2 = 0;  //��ն�ʱ��2����ж�,���ֶ���
        tmr2++;
        if (tmr2 == 100)  // 200msʱ�䵽
        {
            tmr2 = 0;
            SCK  = ~SCK;  // P17��ƽָʾ���
        }
    }
#endif

#ifdef T2_CAP2
    if (EXF2)  // T2ex��ƽ�仯�ж��жϱ�־
    {
        Cap_2[0] = RCAP2;  // T2EX
        if (AIN1)
        {
            // printf("P1.1 Rising %04x\n", Cap_2[0]);
        }
        else
        {
            // printf("P1.1 Falling %04x\n", Cap_2[0]);
        }

        EXF2 = 0;  //���T2ex��׽�жϱ�־
        TH2  = 0;
        TL2  = 0;
    }
#endif
}