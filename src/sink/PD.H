
/********************************** (C) COPYRIGHT *******************************
* File Name          : PD.H
* Author             : WCH
* Version            : V1.0
* Date               : 2018/11/12
* Description        : CH549 Type C CCͨѶ
*******************************************************************************/

#ifndef	__PD_H__
#define __PD_H__

#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"

/* CC�ż�⵽��ADCֵ�жϹ������� */
#define  DufaultPowerMin  (342-137)
#define  DufaultPowerMax  (342+158)
#define  Power1_5AMin     (771-198)
#define  Power1_5AMax     (771+180)
#define  Power3_0AMin     (1383-310)
#define  Power3_0AMax     (1383+288) 

/* ���ݴ�С���� */
#define  RcvBufMaxLen	   73                                         //���ջ�����RcvDataBuf��С���޸ĸ�ֵ���ڿ��ļ���Ч��
#define  SndBufMaxLen	   73                                         //���ͻ�����SndDataBuf��С���޸ĸ�ֵ���ڿ��ļ���Ч��

/* Headerλ���� */
#define  DataRoleUFP       0
#define  DataRoleDFP       1
#define  PwrRoleSink       0
#define  PwrRoleSource     1

/* Message Type : Data Message */
#define  SourceSendCap	   0x01		                                   // 5B:00001
#define  GoodCRC     	   0x01		                                   // 5B:00001
#define  SinkSendRequest   0x02                                        // 5B:00010
#define  Accept            0x03                                        // 5B:00011
#define  REJECT            0x04                                        // 5B:00100
#define  SinkCap           0x04                                        // 5B:00100
#define  PS_RDY  		   0x06		                                   // 5B:00110
#define  GetSourceCap      0x07                                        // 5B:00111
#define  GetSinkCap	       0x08		                                   // 5B:01000
#define  PRSwap            0x0A                                        // 5B:01010
#define  SoftRst		   0x0D		                                   // 5B:01101
#define  VDef		       0x0F

/* Vendor Define Message Command */
#define  DiscIdent		   0x01
#define  DiscSVID		   0x02
#define  DiscMode		   0x03
#define  EnterMode		   0x04
#define  ExitMode		   0x05
#define  Attention		   0x06                                        //SINK����֪ͨ����ʾ�豸����״̬
#define  DPStatUpdate	   0x10
#define  DPConfig		   0x11

#define  REQ			   0x00
#define  ACK			   0x01

/* ����������λ��4B5B �� */
#define  Sync1		       0x18		                                   // 5B:11000
#define  Sync2		       0x11		                                   // 5B:10001
#define  RST1		       0x07		                                   // 5B:00111
#define  RST2		       0x19		                                   // 5B:11001
#define  EOP			   0x0D		                                   // 5B:01101
#define  Sync3		       0x06		                                   // 5B:00110

/* Start of Packet Sequences */
#define  SOP               0                                           //Start of Packet Sequence(SOP)
#define  SOPP              1                                           //Start of Packet Sequence Prime(SOP')
#define  SOPDP             2                                           //Start of Packet Sequence Double Prime(SOP'')
#define  SOPPD             3                                           //Start of Packet Sequence Prime Double(SOP'_Debug)
#define  SOPDPD            4                                           //Start of Packet Sequence Double Prime Double(SOP''_Debug)

/* ��������״̬ */
#define  REVSUCCESS        0x00                                        //�յ�����
#define  NODATA            0x01                                        //������
#define  ILLEGAL           0x02                                        //�յ��Ƿ����ݰ���������SOP'

/* ����״̬ */
#define  DFP_PD_CONNECT    0x00
#define  DFP_PD_DISCONNECT 0x01

/* Header */
typedef struct  {
//	Extended	  1		0
//	NDO			  3
//	MsgID		  3
//	PortPwrRole	  1		0:Sink  1:Source
//	SpecRev		  2		01 Rev2.0
//	PortDataRole  1		0:UFP  1:DFP
//	MessageType	  5

	UINT8 PortPwrRole:1;
	UINT8 MsgID:3;
	UINT8 NDO:3;
	UINT8 Extended:1;
	
	UINT8 MsgType:5;
	UINT8 PortDataRole:1;
	UINT8 SpecRev:2;
	
} _Msg_Header_Struct;

typedef union {				                                              /* Header */
	_Msg_Header_Struct  HeaderStruct;
	UINT8 HeaderData[2];
} _Union_Header;

/* ��ѹ�������� */
typedef struct  {
//	Data	12bit		
//	Volt	10bit		Voltage *0.05V
//	Curr	10bit		Current *0.01A

	UINT16 VoltH4:4;
	UINT16 Data:12;

	UINT16  Current:10;
	UINT16  VoltL6:6;

} _SRC_Cap_Struct;
typedef union {				                                              /* Src Cap */
	_SRC_Cap_Struct  SrcCapStruct;
	UINT8 SrcCapData[4];
} _Union_SrcCap;

/* VDM Header */
typedef struct  {
//	SVID			16		0xFF01: DisplayPort
//
//	StructuredVDM	1
//	SVDMVer			2		00: Ver.1
//	----			5
//	CommandType		2		00: REQ		01: ACK
//	----			1
//	Command			5
	
	UINT16 SVID:16;
	
	UINT16 Command:5;
	UINT16 :1;
	UINT16 CommandType:2;
	UINT16 ModeIndex:3;
	UINT16 :2;
	UINT16 SVDMVer:2;
	UINT16 StructuredVDM:1;
	
} _VDM_Hdr_Struct;
typedef union {
	_VDM_Hdr_Struct  VDMHdrStruct;
	UINT8 VDMHdrData[4];
} _Union_VDM_Hdr;


/* �ⲿ���ñ��� */
extern UINT8X CCSel;                                                     //ѡ��CC�� 1��CC1  2��CC2
extern UINT8X RECV_SOP;                                                  //��ʾ��ǰ���յ���SOP����
extern _Union_Header xdata *Union_Header;                                //ָ��Header��ָ��
extern _Union_SrcCap xdata *Union_SrcCap;                                //ָ���ѹ�����Խṹ
extern _Union_VDM_Hdr xdata *Union_VDM_Hdr;                              //VDMͷ

extern UINT8X RcvDataBuf[];                                              //���ջ�����
extern UINT8X RcvDataCount;                                              //���ջ��������ݸ���

extern UINT8X SndDataBuf[];                                              //���ͻ�����
extern UINT8X SndDataCount;                                              //���ͻ��������ݸ���

/* �ⲿ�Ӻ��� */
extern void  Timer0Init( );                                             //��ʱ��0��ʼ����ʹ�ÿ���ʱ�ӣ�
extern void  ResetSndHeader ( void );                                   //��ʼ������ͷĬ��ֵ(PD2.0�汾�š�MsgID�ѳ�ʼ�����û��ɸ�����Ҫ����PortDataRole��PortPwrRole��ֵ)
extern UINT8 SendHandle ( UINT8 CC_Sel,UINT8 Sop );                     //����SndDataBuf����
extern UINT8 ReceiveHandle ( UINT8 CC_Sel ) ;                           //��ѯ���գ�0x00:���յ����ݰ���0x01:û���յ���0x02:�յ��Ƿ����ݡ����ݱ�����RcvDataBuf

#endif