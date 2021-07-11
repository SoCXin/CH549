/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2019/07/22
* Description        : CH549 ADC��ʼ����ADC�жϺͲ�ѯ��ʽ��ʾ�ⲿͨ���Լ��ڲ��¶ȼ��
*******************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include "ADC.H"
#pragma  NOAREGS
volatile UINT8 GetValueFlag = 0;                                           //ADC�жϷ�ʽ�ɼ���ɱ�־λ��1��Ч
void main( )
{
    UINT8 ch;
    CfgFsys( );                                                            //CH549ʱ��ѡ������24MHz
    mDelaymS(5);                                                           //�Լ���ʱ�ȴ���ʱ�ȶ�
    mInitSTDIO( );                                                         //����0��ʼ��
    printf("ADC demo start ...\n");
#if 1
    ADC_ExInit( 3 );                                                       //ADC��ʼ��,ѡ�����ʱ��
    while(1)
    {
#if (EN_ADC_INT == 0)                                                      //��ѯ��ʽ��ʾADCͨ���л��Ͳɼ�
        for(ch=0; ch!=16; ch++)
        {
            ADC_ChSelect(ch);                                               //ѡ��ͨ��
            ADC_StartSample();                                                 //��������
            while((ADC_CTRL&bADC_IF) == 0)
            {
                ;    //��ѯ�ȴ���־��λ
            }
            ADC_CTRL = bADC_IF;                                                //���־
            printf("ch%d=%d  ",(UINT16)ch,(UINT16)ADC_DAT);                    //���ADC����ֵ
        }
        printf("\n");
        printf("\n");
        mDelaymS(1000);
#else                                                                          //�жϷ�ʽ��ʾADCͨ���л��Ͳɼ�����ͨ��0Ϊ��
       ADC_StartSample();                                                      //��������				
       mDelaymS(10);                                                           //ģ�ⵥƬ������������							
       if(GetValueFlag)                                                        //ͨ���ɼ���ɱ�־
       {
          printf("ch0  \n",(UINT16)ADC_DAT);                                   //���ADC����ֵ		 
       }
       mDelaymS(1000);                                                         //ģ�ⵥƬ������������					 
#endif
    }
#else                                                                         //�¶ȼ��
    ADC_InTSInit();                                                           //��ʼ���¶ȼ��
    while(1)
    {
        ADC_StartSample();                                                    //��������
#if EN_ADC_INT
        mDelaymS(20);                                                         //�жϺ���������Ҳ�������ӱ�־λ
#else
        while((ADC_CTRL&bADC_IF) == 0)
        {
            ;    //��ѯ�ȴ���־��λ
        }
        ADC_CTRL = bADC_IF;                                                   //���־
#endif
        printf("%d ",(UINT16)ADC_DAT);                                        //���ADC����ֵ
        printf("\n");
    }
#endif
}