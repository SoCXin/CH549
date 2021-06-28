/********************************** (C) COPYRIGHT *******************************
 * File Name          : GPIO.C
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/08/09
 * Description        : CH547 GPIO��غ���
 *******************************************************************************/
#include ".\GPIO\GPIO.H"
#pragma NOAREGS
/*******************************************************************************
 * Function Name  : GPIO_Init(UINT8 PORTx,UINT8 PINx,UINT8 MODEx)
 * Description    : GPIO�˿ڳ�ʼ������
 * Input          : PORTx:0~4
 *                  PINx:λ��,ÿ��λ��Ӧ��Port��һ������
 *                  MODEx:
 *                        0:��������ģʽ������û����������
 *                        1:�������ģʽ�����жԳ�������������������������սϴ����
 *                        2:��©�����֧�ָ������룬����û����������
 *                        3:׼˫��ģʽ(��׼ 8051)����©�����֧�����룬��������������(Ĭ��ģʽ)
 * Output         : None
 * Return         : None
 *******************************************************************************/
void GPIO_Init(UINT8 PORTx, UINT8 PINx, UINT8 MODEx)
{
    UINT8 Px_DIR_PU, Px_MOD_OC;
    switch (PORTx)  //������ʼֵ
    {
        case PORT0:
            Px_MOD_OC = P0_MOD_OC;
            Px_DIR_PU = P0_DIR_PU;
            break;
        case PORT1:
            Px_MOD_OC = P1_MOD_OC;
            Px_DIR_PU = P1_DIR_PU;
            break;
        case PORT2:
            Px_MOD_OC = P2_MOD_OC;
            Px_DIR_PU = P2_DIR_PU;
            break;
        case PORT3:
            Px_MOD_OC = P3_MOD_OC;
            Px_DIR_PU = P3_DIR_PU;
            break;
        case PORT4:
            Px_MOD_OC = P4_MOD_OC;
            Px_DIR_PU = P4_DIR_PU;
            break;
        default:
            break;
    }
    switch (MODEx)
    {
        case MODE0:  //��������ģʽ������û����������
            Px_MOD_OC &= ~PINx;
            Px_DIR_PU &= ~PINx;
            break;
        case MODE1:  //�������ģʽ�����жԳ�������������������������սϴ����
            Px_MOD_OC &= ~PINx;
            Px_DIR_PU |= PINx;
            break;
        case MODE2:  //��©�����֧�ָ������룬����û����������
            Px_MOD_OC |= PINx;
            Px_DIR_PU &= ~PINx;
            break;
        case MODE3:  //׼˫��ģʽ(��׼ 8051)����©�����֧�����룬��������������
            Px_MOD_OC |= PINx;
            Px_DIR_PU |= PINx;
            break;
        default:
            break;
    }
    switch (PORTx)  //��д
    {
        case PORT0:
            P0_MOD_OC = Px_MOD_OC;
            P0_DIR_PU = Px_DIR_PU;
            break;
        case PORT1:
            P1_MOD_OC = Px_MOD_OC;
            P1_DIR_PU = Px_DIR_PU;
            break;
        case PORT2:
            P2_MOD_OC = Px_MOD_OC;
            P2_DIR_PU = Px_DIR_PU;
            break;
        case PORT3:
            P3_MOD_OC = Px_MOD_OC;
            P3_DIR_PU = Px_DIR_PU;
            break;
        case PORT4:
            P4_MOD_OC = Px_MOD_OC;
            P4_DIR_PU = Px_DIR_PU;
            break;
        default:
            break;
    }
}
/*******************************************************************************
 * Function Name  : GPIO_INT_Init
 * Description    : ������ RXD1_L��P15_L��P14_L��P03_L��P57_H��P46_L��RXD0_L ��չ���ŵ��ⲿ�ж�
 *                  ͬʱ����������C51�� INT1_L��INT0_L ���ⲿ�жϴ���
 *                  (RXD1_L��RXD0_L�������ĸ�����ȡ���������Ƿ�ӳ��)
 * Input          : IntSrc:��9���ⲿ�жϣ���λ���ʾ�����嶨���GPIO.H
 *                  Mode��0����ƽģʽ   1������ģʽ (ע����չ���ŵ��ж�ģʽ��ͳһ���õ�)
 *                  NewState��0���رն�Ӧ�ⲿ�ж�ʹ��  1��������Ӧ�ⲿ�ж�
 * Output         : None
 * Return         : None
 *******************************************************************************/
void GPIO_INT_Init(UINT16 IntSrc, UINT8 Mode, UINT8 NewState)
{
    /* �жϴ���ģʽ���� */
    if (Mode == INT_EDGE)  //���ش���ģʽ
    {
        if (IntSrc & 0x7F)  //������չ�ж�
        {
            GPIO_IE |= bIE_IO_EDGE;
        }
        if (IntSrc & INT_INT0_L)  //�����ⲿ�ж�0
        {
            IT0 = 1;
        }
        if (IntSrc & INT_INT1_L)  //�����ⲿ�ж�1
        {
            IT1 = 1;
        }
        if (IntSrc & INT_INT3_L)  //�����ⲿ�ж�3
        {
            INTX |= bIT3;
        }
    }
    else  //��ƽ����ģʽ
    {
        if (IntSrc & 0x7F)  //������չ�ж�
        {
            GPIO_IE &= ~bIE_IO_EDGE;
        }
        if (IntSrc & INT_INT0_L)  //�����ⲿ�ж�0
        {
            IT0 = 1;
        }
        if (IntSrc & INT_INT1_L)  //�����ⲿ�ж�1
        {
            IT1 = 1;
        }
        if (IntSrc & INT_INT3_L)  //�����ⲿ�ж�3
        {
            INTX &= ~bIT3;
        }
    }
    /* �ж�ʹ��״̬ */
    if (NewState == Enable)  //�����ⲿ�ж�
    {
        GPIO_IE |= ((UINT8)IntSrc & 0x7F);
        if (IntSrc & INT_INT0_L)  //�����ⲿ�ж�0
        {
            EX0 = 1;
        }
        if (IntSrc & INT_INT1_L)  //�����ⲿ�ж�1
        {
            EX1 = 1;
        }
        if (GPIO_IE & 0x7F)
        {
            IE_GPIO = 1;  //������չGPIO�ж�
        }
        if (IntSrc & INT_INT3_L)  //�����ⲿ�ж�3
        {
            IE_INT3 = 1;
        }
        EA = 1;  //�������ж�
    }
    else  //�رն�Ӧ�ⲿ�ж�
    {
        GPIO_IE &= ~((UINT8)IntSrc & 0x7F);
        if (IntSrc & INT_INT0_L)  //�����ⲿ�ж�0
        {
            EX0 = 0;
        }
        if (IntSrc & INT_INT1_L)  //�����ⲿ�ж�1
        {
            EX1 = 0;
        }
        if ((GPIO_IE & 0x7F) == 0)
        {
            IE_GPIO = 0;  //�ر���չGPIO�ж�
        }
        if (IntSrc & INT_INT3_L)  //�����ⲿ�ж�3
        {
            IE_INT3 = 0;
        }
    }
}
/*******************************************************************************
 * Function Name  : GPIO_ISR
 * Description    : RXD1��P15��P14��P03��P57��P46��RXD0 �����ⲿ�жϷ�����
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void GPIO_EXT_ISR(void) interrupt INT_NO_GPIO
{
    if (AIN11 == 0)
    {
        mDelaymS(10);
        // printf("P03 Falling\n");
    }

    if (AIN5 == 0)
    {
        mDelaymS(10);
        // printf("P15 Falling\n");
    }
}
/*******************************************************************************
 * Function Name  : GPIO_STD0_ISR
 * Description    : INT0(P32) �����ⲿ�жϷ�����
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void GPIO_STD0_ISR(void) interrupt INT_NO_INT0
{
    mDelaymS(10);
    // printf("P32 Falling\n");
}
/*******************************************************************************
 * Function Name  : GPIO_STD1_ISR
 * Description    : INT1(P33) �����ⲿ�жϷ�����
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void GPIO_STD1_ISR(void) interrupt INT_NO_INT1
{
    mDelaymS(10);
    // printf("P33 Falling\n");
}
/*******************************************************************************
 * Function Name  : GPIO_STD1_ISR
 * Description    : INT1(P33) �����ⲿ�жϷ�����
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void GPIO_STD3_ISR(void) interrupt INT_NO_INT3
{
    if (INT3)
    {
        // printf("P37 Rising\n");
        INTX &= ~bIX3;
    }
    else
    {
        // printf("P37 Falling\n");
        INTX |= bIX3;
    }

}