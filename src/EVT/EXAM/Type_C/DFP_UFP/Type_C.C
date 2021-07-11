/********************************** (C) COPYRIGHT *******************************
* File Name          : Type_C.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/27
* Description        : CH549 Type-Cʹ��
                       ��ģʽ �豸�������⡢������֪ͨ���豸
                       ��ģʽ ���������������
*******************************************************************************/
#include "Type_C.H"
#pragma  NOAREGS
/********************************************************************************
DFP (Downstream Facing Port) Host��
UFP (Upstream Facing Port)   Dev��
��DFP��CC pin������������Rp,��UFP������������Rd
��DFP��UFPδ����ǰ,DFP��VBUS��û�������
CC PIN��������������壬��DPF��������CC1�ӵ�������������;���CC2�ӵ��������Ƿ��壻
     �����������֮�󣬾ͻ������Ӧ��USB�ź�

DPF�ڲ�ͬ��ģʽ�£���������CC PINҪ�����ĵ�������Ҫ�ö���Rpֵ
********************************************************************************/
#ifdef TYPE_C_DFP
/*******************************************************************************
* Function Name  : TypeC_DFP_Init(UINT8 Power)
* Description    : Type-C UPF����ʼ��
* Input          : UINT8 Power
                   0 ��ֹUCC1&2����
                   1 Ĭ�ϵ���
                   2 1.5A
                   3 3.0A
* Output         : None
* Return         : NONE
*******************************************************************************/
void TypeC_DFP_Init( UINT8 Power )
{
    P1_MOD_OC &= ~(bUCC2|bUCC1);
    P1_DIR_PU &= ~(bUCC2|bUCC1);                                                     //UCC1 UCC2 ���ø�������
    VCTL = 1;
    P1_MOD_OC &= ~(bVCTL);                                                           //bVCTL ����������ߵ�ƽ��ֹ���VCTL
    P1_DIR_PU |= (bVCTL);
    if(Power == 0)
    {
        DFP_DisableRpUCC1();                                                         //UCC1��ֹ
        DFP_DisableRpUCC2();                                                         //UCC2��ֹ
    }
    else if(Power == 1)
    {
        DFP_DefaultPowerUCC1();                                                      //�������Ĭ��
        DFP_DefaultPowerUCC2();
    }
    else if(Power == 2)
    {
        DFP_1_5APowerUCC1();                                                         //�������1.5A
        DFP_1_5APowerUCC2();
    }
    else if(Power == 3)
    {
        DFP_3_0APowerUCC1();                                                         //�������3.0A
        DFP_3_0APowerUCC2();
    }
    ADC_CFG |= bADC_EN | bADC_AIN_EN;                                                //����ADCģ���Դ,�����ⲿͨ��
    ADC_CFG = ADC_CFG & ~(bADC_CLK0 | bADC_CLK1);                                    //ѡ��ADC�ο�ʱ�� 750K
    ADC_CTRL = bADC_IF;                                                              //���ADCת����ɱ�־��д1����
    mDelayuS(2);                                                                     //�ȴ�ADC��Դ�ȶ�
}
/*******************************************************************************
* Function Name  : TypeC_DFP_Insert(void)
* Description    : Type-C DPF���UFP���壬�����Լ�δ������Ѳ���
* Input          : NONE
* Output         : None
* Return         : 0   δ����
                   1   ��������
                   2   ��������
                   3   ���ӣ��޷��ж�����
*******************************************************************************/
UINT8 TypeC_DFP_Insert( void )
{
    UINT16 UCC1_Value,UCC2_Value;
    ADC_CHAN = 4;                                                                   //CC1����������AIN4(P14)
    ADC_CTRL = bADC_START;                                                          //��������
    while((ADC_CTRL&bADC_IF) == 0)
    {
        ;    //��ѯ�ȴ���־��λ
    }
    ADC_CTRL = bADC_IF;                                                             //���־
    UCC1_Value = ADC_DAT&0xFFF;
    ADC_CHAN = 5;                                                                   //CC2����������AIN5(P15)
    ADC_CTRL = bADC_START;                                                          //��������
    while((ADC_CTRL&bADC_IF) == 0)
    {
        ;    //��ѯ�ȴ���־��λ
    }
    ADC_CTRL = bADC_IF;                                                             //���־
    UCC2_Value = (ADC_DAT&0xFFF);
    printf("UCC1:%d UCC2:%d\n",(UINT16)UCC1_Value,(UINT16)UCC2_Value);
    if( (UCC1_Value<=UCC_Connect_Vlaue) && (UCC2_Value<=UCC_Connect_Vlaue) )        //˫������
    {
        return UCC_CONNECT;
    }
    else if( (UCC1_Value<=UCC_Connect_Vlaue) && (UCC2_Value>=UCC_Connect_Vlaue) )   //��������
    {
        return UCC1_CONNECT;
    }
    else if( (UCC1_Value>=UCC_Connect_Vlaue) && (UCC2_Value<=UCC_Connect_Vlaue) )   //��������
    {
        return UCC2_CONNECT;
    }
    else if( (UCC1_Value>=UCC_Connect_Vlaue) && (UCC2_Value>=UCC_Connect_Vlaue) )   //δ����
    {
        return UCC_DISCONNECT;
    }
    return UCC_DISCONNECT;
}
#endif
/********************************************************************************
UPF,��Ҫ���CC�ܽŵĵ�ѹֵ����ȡDFP�ĵ����������
          CC��ѹMin      CC��ѹMax
Ĭ�ϵ���    0.25V         0.61V
1.5A        0.70V         1.16V
3.0A        1.31V         2.04V
********************************************************************************/
#ifdef TYPE_C_UFP
/*******************************************************************************
* Function Name  : TypeC_UPF_PDInit()
* Description    : Type-C UPF��ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TypeC_UPF_PDInit( void )
{
    P1_MOD_OC &= ~(bUCC2|bUCC1);                                                    //UCC1 UCC2 ���ø�������
    P1_DIR_PU &= ~(bUCC2|bUCC1);
    UPF_CC1RdCfg(1);                                                                //����UCC1��������5.1K
    ADC_CFG |= bADC_EN | bADC_AIN_EN;                                               //����ADCģ���Դ,�����ⲿͨ��
    ADC_CFG = ADC_CFG & ~(bADC_CLK0 | bADC_CLK1);                                   //ѡ��ADC�ο�ʱ�� 750Khz
    ADC_CTRL = bADC_IF;                                                             //���ADCת����ɱ�־��д1����
    mDelayuS(2);                                                                    //�ȴ�ADC��Դ�ȶ�
}
/*******************************************************************************
* Function Name  : TypeC_UPF_PDCheck()
* Description    : Type-C UPF���DPF��������
* Input          : None
* Output         : None
* Return         : UINT8
                   1 Ĭ�ϵ���
                   2 1.5A
                   3 3.0A
                   0xff ����
*******************************************************************************/
UINT8 TypeC_UPF_PDCheck()
{
    UINT16 UCC1_Value;
    ADC_CHAN = 4;                                                                   //CC1����������AIN4(P14)
    ADC_CTRL = bADC_START;                                                          //��������
    while((ADC_CTRL&bADC_IF) == 0)
    {
        ;    //��ѯ�ȴ���־��λ
    }
    ADC_CTRL = bADC_IF;                                                             //���־
    UCC1_Value = ADC_DAT&0xFFF;
    printf("UCC1=%d\n",(UINT16)UCC1_Value);
    if((UCC1_Value >= Power3_0AMin)&&(UCC1_Value <= Power3_0AMax))
    {
        return UPF_PD_3A;                                                           //3.0A��������
    }
    else if((UCC1_Value >= Power1_5AMin)&&(UCC1_Value <= Power1_5AMax))
    {
        return UPF_PD_1A5;                                                          //1.5A��������
    }
    else if((UCC1_Value >= DufaultPowerMin)&&(UCC1_Value <= DufaultPowerMax))
    {
        return UPF_PD_Normal;                                                       //Ĭ�Ϲ�������
    }
    return UPD_PD_DISCONNECT;
}
#endif