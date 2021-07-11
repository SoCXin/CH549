/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/21
* Description        : CH549 Time ��ʼ������ʱ������������ֵ��T2��׽���ܵ�
                       ��ʱ���жϴ���
                       ע�����DEBUG.C
*******************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include ".\Timer\Timer.H"
#pragma  NOAREGS
void main( )
{
    CfgFsys( );                                                                //CH549ʱ��ѡ������
    mDelaymS(5);                                                               //�޸���Ƶ�������Լ���ʱ�ȴ���Ƶ�ȶ�
    mInitSTDIO( );                                                             //���ڳ�ʼ��
    printf("Timer Demo start ...\n");
    P1_MOD_OC &= ~(1<<7);                                                      //SCK�������������������
    P1_DIR_PU |= (1<<7);
#ifdef T0_INT
    printf("T0 Test ...\n");
    mTimer0Clk12DivFsys();                                                     //T0��ʱ��ʱ������ FREQ_SYS/12
    mTimer_x_ModInit(0,1);                                                     //T0 ��ʱ��ģʽ���� ģʽ1 16λ��ʱ��
    mTimer_x_SetData(0,2000);                                                  //T0��ʱ����ֵ 24MHZ 1MS�ж�
    mTimer0RunCTL(1);                                                          //T0��ʱ������
    ET0 = 1;                                                                   //T0��ʱ���жϿ���
    EA = 1;
#endif
#ifdef T1_INT                                                                  //ע�⣺DEBUG.C��mInitSTDIOʹ��T1��ģʽ2��Ϊ�����ʷ�����
    printf("T1 Test ...\n");
    mTimer1Clk12DivFsys( );                                                    //T1��ʱ��ʱ������ FREQ_SYS/12
    mTimer_x_ModInit(1,2);                                                     //T1 ��ʱ��ģʽ����,ģʽ2 8λ�Զ���װ
    mTimer_x_SetData(1,0xC7C8);                                                //T1��ʱ����ֵ TH1=TL1=256-200=38H;  100us�ж�
    mTimer1RunCTL(1);                                                          //T1��ʱ������
    ET1 = 1;                                                                   //T1��ʱ���жϿ���
    EA = 1;
#endif
#ifdef T2_INT
    printf("T2 Test ...\n");
    mTimer2ClkFsys();                                                          //T2��ʱ��ʱ������  ����FREQ_SYS
    mTimer_x_ModInit(2,0);                                                     //T2 ��ʱ��ģʽ����
    mTimer_x_SetData(2,48000);                                                 //T2��ʱ����ֵ FREQ_SYS=24MHz,2ms�ж�
    mTimer2RunCTL(1);                                                          //T2��ʱ������
    ET2 = 1;                                                                   //T2��ʱ���жϿ���
    EA = 1;
#endif
//����ͬʱ����T0����SCK��������������
#ifdef T2_CAP
    printf("T2_CAP Test ...\n");
    P1_MOD_OC &= ~(1<<1);                                                     //CAP2 ��������
    P1_DIR_PU &= ~(1<<1);
    P1_MOD_OC &= ~(1<<0);                                                     //CAP1 ��������
    P1_DIR_PU &= ~(1<<0);
    P3_MOD_OC &= ~(1<<6);                                                     //CAP0 ��������
    P3_DIR_PU &= ~(1<<6);
    mTimer2Clk12DivFsys();                                                    //T2��ʱ��ʱ������ FREQ_SYS/12
    mTimer_x_SetData(2,0);                                                    //T2 ��ʱ��ģʽ���ò�׽ģʽ
    CAP2Init(1);                                                              //T2 CAP2(P11)���ã������ز�׽
    CAP1Init(1);                                                              //T2 CAP1(P10)���ã������ز�׽
    CAP0Init(1);                                                              //T2 CAP0(P36)���ã������ز�׽
    mTimer2RunCTL(1);                                                         //T2��ʱ������
    ET2 = 1;                                                                  //T2��ʱ���жϿ���
    EA = 1;
#endif
    while(1)
    {
    }
}