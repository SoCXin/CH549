#ifndef __TYPE_C_H__
#define __TYPE_C_H__
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#define     TYPE_C_DFP
//#define     TYPE_C_UFP
sbit VCTL = P1^1;
#define bVCTL 0x02
/* CC�ż�⵽��ADCֵ�ж�DFP�Ĺ������� */
#define  DufaultPowerMin  (342-137)
#define  DufaultPowerMax  (342+158)
#define  Power1_5AMin     (771-198)
#define  Power1_5AMax     (771+180)
#define  Power3_0AMin     (1383-310)
#define  Power3_0AMax     (1383+288)
/* DFP ��ض��� */
#ifdef TYPE_C_DFP
#define DFP_DisableRpUCC1()     {USB_C_CTRL = ~(bUCC1_PU1_EN | bUCC1_PU0_EN);}
#define DFP_DefaultPowerUCC1()  {USB_C_CTRL = USB_C_CTRL & ~bUCC1_PU1_EN | bUCC1_PU0_EN;}
#define DFP_1_5APowerUCC1()     {USB_C_CTRL = USB_C_CTRL | bUCC1_PU1_EN & ~bUCC1_PU0_EN; }
#define DFP_3_0APowerUCC1()     {USB_C_CTRL |= bUCC1_PU1_EN | bUCC1_PU0_EN;}
#define DFP_DisableRpUCC2()     {USB_C_CTRL = ~(bUCC2_PU1_EN | bUCC2_PU0_EN);}
#define DFP_DefaultPowerUCC2()  {USB_C_CTRL = USB_C_CTRL & ~bUCC2_PU1_EN | bUCC2_PU0_EN;}
#define DFP_1_5APowerUCC2()     {USB_C_CTRL = USB_C_CTRL | bUCC2_PU1_EN & ~bUCC2_PU0_EN;}
#define DFP_3_0APowerUCC2()     {USB_C_CTRL |= bUCC2_PU1_EN | bUCC2_PU0_EN;}
#define UCC_Connect_Vlaue       Power3_0AMax
#define UCC_DISCONNECT          0x00                                                       //�豸δ����
#define UCC1_CONNECT            0x01                                                       //��������
#define UCC2_CONNECT            0x02                                                       //��������
#define UCC_CONNECT             0x03                                                       //UFP C1��C2������5.1K����
extern void  TypeC_DFP_Init( UINT8 Power );                                                //UPF��ʼ��,���������������
extern UINT8 TypeC_DFP_Insert( void );                                                     //DPF���UFP���壬�����Լ�δ������Ѳ���
#endif
/* UFP ��ض��� */
#ifdef TYPE_C_UFP
#define UPF_CC1RdCfg(SS)        (USB_C_CTRL = SS ? (USB_C_CTRL|bUCC1_PD_EN) : 0)           //UCC1 ����Rd ������ر�
#define UPF_CC2RdCfg(SS)        (USB_C_CTRL = SS ? (USB_C_CTRL|bUCC2_PD_EN) : 0)           //UCC2 ����Rd ������ر�
#define UPF_PD_Normal           0x01                                                       //Ĭ�Ϲ�������500mA
#define UPF_PD_1A5              0x02                                                       //��������1.5mA
#define UPF_PD_3A               0x03                                                       //��������3mA
#define UPD_PD_DISCONNECT       0xff                                                       //�豸δ����
extern void  TypeC_UPF_PDInit( void );                                                     //Type-C UPF��ʼ��
extern UINT8 TypeC_UPF_PDCheck();                                                          //Type-C UPF���DPF��������
#endif
#endif