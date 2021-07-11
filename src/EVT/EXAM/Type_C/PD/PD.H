/********************************** (C) COPYRIGHT *******************************
* File Name          : PD.H
* Author             : WCH
* Version            : V1.00
* Date               : 018/09/15
* Description        :
*******************************************************************************/
#ifndef __PD_H__
#define __PD_H__
#include "CH549.H"
#include "DEBUG.H"
/*******************************************************************************
 *     GPIO            Function
 *     P14               CC1
 *     P15               CC2
 *     P11               PD ͨѶ����״̬���
 *******************************************************************************/

/* ��Դ���ƿ���/���״ָ̬ʾ
    �ض����Ϊ0�������Ϊ1
*/
sbit VBUSG = P1^1;
/* ��Դ��ɫ�궨�� */
#define     SINK                    0
#define     SOURCE                  1
/* ��������ѡ��SOURCE ��SINK ֻ��ѡ��1�� */
#define     POWER_ROLE              SINK
#define     CC_CHAN_1               1
#define     CC_CHAN_2               2
#define     CC_AD_CHAN_1            4
#define     CC_AD_CHAN_2            5
/* CC�ż�⵽��ADCֵ�жϹ������� */
/*  ʵ�ʵ�ѹ        �������        ADCʵ��ֵ(ADC��ƫ��ѹ5V��12λ)
    ���ǵ�CC������5V����3.3V�����������������CC�߼���ѹҪ��������
 * +---------+----------+-------+
 * | 0.4 V   | Default  | 342   |
 * | 0.9 V   | 1.5A     | 771   |
 * | 1.6 V   | 3.0A     | 1383  |
 * +---------+----------+-------+

*/
#define     DEFAULT_POWER_MIN       (342-137)
#define     DEFAULT_POWER_MAX       (342+158)
#define     POWER_1A5_MIN           (771-198)
#define     POWER_1A5_MAX           (771+180)
#define     POWER_3A0_MIN           (1383-310)
#define     POWER_3A0_MAX           (1383+288)
/* PD ��Ϣ����궨�� */
#define     SOURCE_SEND_CAP         0x01                                            /* 5B:00001 */
#define     GOOD_CRC                0x01                                            /* 5B:00001 */
#define     SINK_SEND_REQUEST       0x02                                            /* 5B:00010 */
#define     SOURCE_SEND_ACCEPT      0x03                                            /* 5B:00011 */
#define     REJECT                  0x04                                            /* 5B:00100 */
#define     PS_RDY                  0x06                                            /* 5B:00110 */
/* DFP����״̬ */
#define     DFP_DEFAULT_CURRENT     0X01
#define     DFP_1_5_A               0X02
#define     DFP_3_0_A               0X03
#define     DFP_DISCONNECT          0XFF
#define     DFP_STAT_DISCONNECT     0X00
#define     DFP_STAT_CONNECT        0X01
/*********************************************************************************************************/
/* SOURCE �˺궨��*/
/* UFP����״̬ */
#define     UCC_DISCONNECT          0x00                                            //�豸δ����
#define     UCC1_CONNECT            0x01                                            //��������
#define     UCC2_CONNECT            0x02                                            //��������
/*
 * ����5����ѹ��������λ��������Source Capability
 * ����Ҫ�ĵ�λ�������ó�0,����5V��������ڣ��ұ���ΪVOLT1,5V�������ɵ�
*/
/* ��ѹ������λ1mV */
#define     VOLT1                   5000
#define     VOLT2                   9000
#define     VOLT3                   15000
#define     VOLT4                   20000
#define     VOLT5                   25000
/* ����������λ1mA */
#define     AMP1                    3000
#define     AMP2                    1800
#define     AMP3                    1500
#define     AMP4                    1200
#define     AMP5                    1000
/* �ⲿ���ñ��� */
extern UINT8X CCSel;                                                                /* PDͨѶCC���ӹܽ�: 1��CC1  2��CC2 */
/* �ⲿ�Ӻ��� */
/*******************************************************************************
* Function Name  : Timer0Init()
* Description    : ����ʹ��Timer0��ʱģʽ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern void  Timer0Init( );
/*******************************************************************************
* Function Name  : PD_Init()
* Description    : Type-C UPFģʽ����,����ADC����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern void  PD_Sink_Init( );
/*******************************************************************************
* Function Name  : Connect_Check(void)
* Description    : UPF���DPF��������
* Input          : None
* Output         : None
                   1 Ĭ�ϵ���
                   2 1.5A
                   3 3.0A
                   0xff ����
*******************************************************************************/
extern UINT8 Connect_Check( void );
/*******************************************************************************
* Function Name  : ResetSndHeader
* Description    : ��ʼ������ͷĬ��ֵ
* Input          : NONE
* Output         : None
* Return         : None
*******************************************************************************/
extern void  ResetSndHeader (void);
/*******************************************************************************
* Function Name  : SendHandle
* Description    : �ײ㷢�ͺ���,����GOODCRC�Ľ���
*                  ���PRE��SOF��CRC32��EOP,Ȼ�����4B5B���롢BMC����
* Input          : CC_Sel,��ʾ���õ�CC��
* Output         : None
* Return         : ����״̬
*                  0x00: ���ͳɹ�
*                  0x01: ����ʧ��
*******************************************************************************/
extern UINT8 SendHandle ( UINT8 CC_Sel );
/*******************************************************************************
* Function Name  : ReceiveHandle
* Description    : �ײ���պ���,����GOODCRC�ķ���
*                  ���PRE��SOF��CRC32��EOP
* Input          : CC_Sel,��ʾ���õ�CC��
* Output         : None
* Return         : 0x00:���յ����ݰ�
*                  0x01:û���յ�
*******************************************************************************/
extern UINT8 ReceiveHandle ( UINT8 CC_Sel ) ;
/*******************************************************************************
* Function Name  : SearchVoltage
* Description    : ����Volt��ѹ������ӽ���
* Input          : Volt,��ѹֵ����λ mV
* Output         : None
* Return         : ���ֵ
*                  �������0xFF,��С��Ҳ������Ĵ��Ҳ��������ѹֵ
*******************************************************************************/
extern UINT8 SearchVoltage ( UINT16 Volt );
/*******************************************************************************
* Function Name  : PrepareReqData
* Description    : ׼��Request�������ݰ�
* Input          : Index : SearchVoltage()�������ص�����ֵ
* Output         : NONE
* Return         : NONE
*******************************************************************************/
extern void PrepareReqData(UINT8 index);
/*******************************************************************************
* Function Name  : GetMsgType
* Description    : ��ȡ��ǰ��Ϣ״̬��ʶ
* Input          : NONE
* Output         : NONE
* Return         : ��Ϣ����
*******************************************************************************/
extern UINT8 GetMsgType(void);
/* PD SOURCE �������� */
/*******************************************************************************
* Function Name  : PD_Source_Init()
* Description    : Type-C UPF����ʼ������ʼΪ3A�����������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern void PD_Source_Init( void );
/*******************************************************************************
* Function Name  : DFP_Insert(void)
* Description    : DPF���UFP���壬�����Լ�δ������Ѳ���
* Input          : None
* Output         : None
* Return         : 0   δ����
                   1   ��������
                   2   ��������
*******************************************************************************/
extern UINT8 DFP_Insert( void );
/*******************************************************************************
* Function Name  : Config_SourceCap
* Description    : ���� Source Capability
* Input          : None
* Return         : None
*******************************************************************************/
extern UINT8 Config_SourceCap(void);
/*******************************************************************************
* Function Name  : SetMsgType
* Description    : ������Ϣ����
* Input          : None
* Return         : None
*******************************************************************************/
extern void SetMsgType(UINT8 t_msgType);
#endif