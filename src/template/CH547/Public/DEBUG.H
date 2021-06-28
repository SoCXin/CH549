/* ���� */
/* �ṩprintf�ӳ������ʱ���� */

#ifndef	__DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>
#include <string.h>

//���庯������ֵ
#ifndef  SUCCESS
#define  SUCCESS  1
#endif
#ifndef  FAIL
#define  FAIL    0
#endif

//���嶨ʱ����ʼ
#ifndef  START
#define  START  1
#endif
#ifndef  STOP
#define  STOP    0
#endif

#ifndef  DE_PRINTF                       //���Կ���
#define  DE_PRINTF     1
#endif

#define	 FREQ_SYS	     24000000	         //ϵͳ��Ƶ24MHz
#define  OSC_EN_XT     0                 //�ⲿ����ʹ�ܣ�Ĭ�Ͽ����ڲ�����

#ifndef  UART0BUAD
#define  UART0BUAD     115200
#endif


void	CfgFsys( );                        // CH547ʱ��ѡ�������
void	mDelayuS( UINT16 n );              // ��uSΪ��λ��ʱ
void	mDelaymS( UINT16 n );              // ��mSΪ��λ��ʱ
void  CH547UART0Alter();                 // CH547����0����ӳ�䵽P0.2/P0.3
void	mInitSTDIO( );                     // T1��Ϊ�����ʷ�����
void  CH547SoftReset( );                 // CH547����λ
void  CH547WDTModeSelect(UINT8 mode);	   // ���Ź�ģʽѡ��
void  CH547WDTFeed(UINT8 tim);           // ι��
	
#endif
