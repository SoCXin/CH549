/********************************** (C) COPYRIGHT *******************************
* File Name          : Compare.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/09
* Description        : CH549 ADC�Ƚ���
*******************************************************************************/
#include "Compare.H"
#pragma  NOAREGS
/*******************************************************************************
* Function Name  : CMP_Init
* Description    : �Ƚ���ģ���ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CMP_Init( void )
{
    ADC_CFG |= bCMP_EN;                     //�����Ƚ���ģ���Դ
    /* �Ƚ�ͨ��ѡ��,�г��������,ʵ��ʹ�ø��ݾ����������� */
#if 1
    /* �ⲿͨ��0���ڲ�1/8 VDD��ѹ�Ƚ� */
    P1_MOD_OC &= ~0x01;                     //P1.0��������
    P1_DIR_PU &= ~0x01;
    ADC_CFG |= (bADC_AIN_EN | bVDD_REF_EN); //�����ⲿͨ��,�����ο���ѹ
    ADC_CHAN = (0<<6)|(3<<4)|(0<<0);        //�Ƚ���-ͨ�����ڲ�ͨ�����ⲿͨ���ֱ� 0,3,0
#endif
#if 0
    /* �ⲿͨ��0���ⲿͨ��1��ѹ�Ƚ� */
    P1_MOD_OC &= ~0x03;                     //P1.0 P1.1��������
    P1_DIR_PU &= ~0x03;
    ADC_CFG |= bADC_AIN_EN;                 //�����ⲿͨ��
    ADC_CHAN = (2<<6)|(3<<4)|(0<<0);        //�Ƚ���-ͨ�����ڲ�ͨ�����ⲿͨ���ֱ� 2,3,0
#endif
#if 0
    /* �ڲ�54.5%��V33 ���ⲿͨ��1�Ƚ� */
    P1_MOD_OC &= ~0x02;                     //P1.1��������
    P1_DIR_PU &= ~0x02;
    ADC_CFG &= ~bADC_AIN_EN;                //�ر��ⲿͨ��
    ADC_CHAN = (2<<6)|(2<<4)|(0<<0);        //�Ƚ���-ͨ�����ڲ�ͨ�����ⲿͨ���ֱ� 2,2,0
#endif
    ADC_CTRL = bCMP_IF;                     //����Ƚϱ�־��д1����
#if EN_ADC_INT
    SCON2 &= bU2IE;                         //��UART2�жϵ�ַ���ã����ж���2ѡ1
    IE_ADC = 1;                             //����ADC�ж�ʹ��
    EA = 1;                                 //�������ж�ʹ��
#endif
}
/*******************************************************************************
* Function Name  : CMP_IN_Minus_Signal_SELT
* Description    : �Ƚ���IN-����ͨ���ź�ѡ��
                   UINT8 mode
                   0:ADC/comparator IN-����VDD/8
                   1:ADC/comparator IN-����VDD
                   2:ADC/comparator IN-����VDD/4
                   3:�Ͽ��ڲ�����,����
                   4:ADC/comparator IN-����P11
                   5:ADC/comparator IN-����P12
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CMP_IN_Minus_Signal_SELT(UINT8 mode)
{
  switch(mode)
  {
    case 0:
      ADC_CFG = ADC_CFG | bVDD_REF_EN | bCMP_EN; //ADC/comparator IN-����VDD/8
      ADC_CHAN &= ~MASK_CMP_CHAN;	
    break;	
    case 1:
      ADC_CFG = ADC_CFG | bVDD_REF_EN & ~bCMP_EN; //ADC/comparator IN-����VDD
      ADC_CHAN = ADC_CHAN & ~MASK_CMP_CHAN | 0x40;	      			
    break;
    case 2:
      ADC_CFG = ADC_CFG | bVDD_REF_EN | bCMP_EN; //ADC/comparator IN-����VDD/4
      ADC_CHAN = ADC_CHAN & ~MASK_CMP_CHAN | 0x40;				
    break;
    case 3:
      ADC_CFG |= bCMP_EN;                        //ADC/comparator IN-����P11
      ADC_CHAN = ADC_CHAN & ~MASK_CMP_CHAN | 0x80;			
    break;
    case 4:
      ADC_CFG |= bCMP_EN;                        //ADC/comparator IN-����P12
      ADC_CHAN |= MASK_CMP_CHAN;				
    break;
    case 5:
    break;
    default:
    break;			
  }
}
/*******************************************************************************
* Function Name  : CMP_Inter_IN_Plus_Signal_SELT
* Description    : �Ƚ���IN+�����ڲ�ͨ���ź�ѡ��
                   UINT8 mode
                   0:ADC/comparator IN+����VDD/2
                   1:ADC/comparator IN+����V33
                   2:ADC/comparator IN+����V33/1.83
                   3:�Ͽ��ڲ�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CMP_Inter_IN_Plus_Signal_SELT(UINT8 mode)
{
    ADC_CHAN = ADC_CHAN & ~MASK_ADC_I_CH | (mode<<4);
    if(mode == 0)
    {
        ADC_CFG |= bVDD_REF_EN;
    }
    if(mode == 3)
    {
        ADC_CFG |= bADC_AIN_EN;
    }
}
/*******************************************************************************
* Function Name  : CMP_Ext_IN_Plus_SELT
* Description    : �Ƚ���IN+�����ⲿ�ź�ͨ��ѡ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CMP_Ext_IN_Plus_SELT(UINT8 ch)
{
    ADC_CFG |= bADC_AIN_EN;
    ADC_CHAN = ADC_CHAN & ~MASK_ADC_CHAN | ch;
}
/*******************************************************************************
* Function Name  : CMP_Ext_IN_Plus_Float
* Description    : �Ƚ���IN+�����ⲿ�ź�ͨ�����ø���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CMP_Ext_IN_Plus_Float( )
{
    ADC_CFG &= ~bADC_AIN_EN;
}
#if EN_ADC_INT
/*******************************************************************************
* Function Name  : ADC_ISR( )
* Description    : ADC �жϷ�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_ISR(void)  interrupt INT_NO_ADC
{
    if(ADC_CTRL&bCMP_IF)
    {
        ADC_CTRL = bCMP_IF;                 //����Ƚ����仯�жϱ�־
        if(ADC_CTRL&bCMPDO)
        {
            printf("+ > -\n");
        }
        else
        {
            printf("+ < -\n");
        }
    }
}
#endif