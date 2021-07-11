#ifndef __SD_H__
#define __SD_H__
#include ".\SPI\SPI.H"                                                 /* ����SPI������������ */
#define SD_CMD_TIMEOUT  0x3FFFF                                        /* ��ʱʱ������ */
/* SD������ܷ��صĴ����� */
#define SD_ERR_CMD_OK                   0x00                           /* ������ִ�гɹ� */
#define SD_ERR_CMD_TIMEOUT              0x6A                           /* ��������Ӧ��ʱ */
#define SD_ERR_INIT                     0xFE                           /* ����ʼ��ʧ�� */
#define SD_CMD_UNSURPORT                0x6B                           /* ��֧�ֵ����� */
#define SD_ERR_CMD_PARAM                0x6C                           /* ����������� */
#define SD_ERR_CMD_CRC                  0x6D                           /* ����CRC���� */
#define SD_ERR_VOL_NOTSUSP              0x6E                           /* CSD���ݳ��� */
#define SD_BLOCK_ERROR                  0x6F                           /* ���С���� */
#define SD_ERR_INSERT                   0x70                           /* ��δ���� */
#define CMD0                            0                              /* ����λ���� */
#define CMD1                            1                              /* sd��һ��ʹ��acmd41��ʼ��������mmc��������CMD1 */
#define CMD8                            8                              /* �ӿڲ�������,�������ֿ����� */
#define CMD9                            9                              /* ��ȡCSD */
#define CMD12                           12                             /* ��ֹ���� R1b */
#define CMD16                           16                             /* ���ÿ鳤�� */
#define CMD17                           17                             /* ����� */
#define CMD18                           18                             /* ���� */
#define CMD24                           24                             /* ����д */
#define CMD25                           25                             /* ���д */
#define CMD55                           55                             /* ACMD֮ǰ��Ҫ */
#define CMD58                           58                             /* ��ȡOCR */
#define ACMD41                          41                             /* ��ʼ������ */
typedef struct SD_INFO_
{
    UINT8  insert;                                                     /* ��α�־ 0���γ�  1������ */
    UINT8  type;                                                       /* 0:mmc v3   1:sd v1   2:sd v2��׼��   3:sd v2��������  4:others(δʶ��) */
    UINT8  init_success;                                               /* ��ʼ�������1 */
    UINT16 block_len;                                                  /* ����block��С */
    UINT32 block_num;                                                  /* ���� */
    UINT32 capability;                                                 /* ��λ:K�ֽ� */
} SD_INFO;
//�ⲿ���ú㶨��
#define  SD_INSERT_PIN_IN( )            P3_4
//�ⲿ���ñ���
extern SD_INFO xdata sd_info;                                          /* SD����Ϣ */
//�ⲿ�����Ӻ���
extern UINT8 SD_Check_Insert( void );
extern UINT8 SD_Init(void);
extern UINT8 SD_READ_SECTOR(UINT8X *datbuf,UINT32 sector_num);
extern UINT8 SD_READ_MULT_SECTOR(UINT8X *datbuf,UINT32 sector_num,UINT16 blocks );
extern UINT8 SD_WRITE_SECTOR(UINT8X *datbuf,UINT32 sector_num);
extern UINT8 SD_WRITE_MULT_SECTOR( UINT8X *datbuf,UINT32 sector_num,UINT16 blocks );
#endif