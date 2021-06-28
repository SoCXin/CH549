/********************************** (C) COPYRIGHT *******************************
* File Name          : Timer.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/21
* Description        : CH547 Time 初始化、定时器、计数器赋值、T2捕捉功能开启函数等
                       定时器中断函数
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
* Description    : CH547定时计数器x模式设置
* Input          : UINT8 mode,Timer模式选择
                   0：模式0，13位定时器，TLn的高3位无效
                   1：模式1，16位定时器
                   2：模式2，8位自动重装定时器
                   3：模式3，两个8位定时器  Timer0
                   3：模式3，Timer1停止
* Output         : None
* Return         : 成功  SUCCESS
                   失败  FAIL
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
        RCLK   = 0;  // 16位自动重载定时器
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
 * Description    : CH547Timer0 TH0和TL0赋值
 * Input          : UINT16 dat;定时器赋值
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
        RCAP2L = TL2 = tmp & 0xff;  // 16位自动重载定时器
        RCAP2H = TH2 = (tmp >> 8) & 0xff;
    }
}
/*******************************************************************************
* Function Name  : CAP2Init(UINT8 mode)
* Description    : CH547定时计数器2 T2EX引脚捕捉功能初始化（CAP2 P11）
                   UINT8 mode,边沿捕捉模式选择
                   0:T2ex从下降沿到下一个下降沿
                   1:T2ex任意边沿之间
                   3:T2ex从上升沿到下一个上升沿
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
    T2MOD |= (mode << 2);  //边沿捕捉模式选择
    CP_RL2 = 1;            //启动T2ex的捕捉功能
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
    mTimer0Clk12DivFsys();      // T0定时器时钟设置 FREQ_SYS/12
    mTimer_x_ModInit(0, 1);     // T0 定时器模式设置 模式1 16位定时器
    mTimer_x_SetData(0, 2000);  // T0定时器赋值 24MHZ 1MS中断
    mTimer0RunCTL(1);           // T0定时器启动
    ET0 = 1;                    // T0定时器中断开启
    EA  = 1;
}
/*******************************************************************************
 * Function Name  : mTimer0Interrupt()
 * Description    : CH547定时计数器0定时计数器中断处理函数 1ms中断
 *******************************************************************************/
void mTimer0Interrupt(void) interrupt INT_NO_TMR0 using 1  // timer0中断服务程序,使用寄存器组1
{
    mTimer_x_SetData(0, 2000);  //模式1 需重新给TH0和TL0赋值
    SCK = ~SCK;                 //大约1ms
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
 * Description    : CH547定时计数器1定时计数器中断处理函数 100us中断
 *******************************************************************************/
void mTimer1Interrupt(void) interrupt INT_NO_TMR1 using 2  // timer1中断服务程序,使用寄存器组2
{
    //方式2时，Timer1自动重装
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
 * Description    : CH547定时计数器0定时计数器中断处理函数
 *******************************************************************************/
void mTimer2Interrupt(void) interrupt INT_NO_TMR2 using 3  // timer2中断服务程序,使用寄存器组3
{
#ifdef T2_INT
    static UINT8 tmr2 = 0;
    if (TF2)
    {
        TF2 = 0;  //清空定时器2溢出中断,需手动请
        tmr2++;
        if (tmr2 == 100)  // 200ms时间到
        {
            tmr2 = 0;
            SCK  = ~SCK;  // P17电平指示监控
        }
    }
#endif

#ifdef T2_CAP2
    if (EXF2)  // T2ex电平变化中断中断标志
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

        EXF2 = 0;  //清空T2ex捕捉中断标志
        TH2  = 0;
        TL2  = 0;
    }
#endif
}
