#ifndef _USER_TYPE_H_
#define _USER_TYPE_H_

#define Version "03.00.01"

typedef struct
{
    unsigned char s0 : 2;
    unsigned char s1 : 2;
    unsigned char s2 : 2;
    unsigned char s3 : 2;
} _STATE_2bits;

typedef struct
{
    unsigned char s0 : 4;
    unsigned char s1 : 4;
} _STATE_4bits;

typedef union
{
    _STATE_2bits s2bits;
    _STATE_4bits s4bits;
    unsigned char byte;
} _USR_FLAGA_type;

typedef struct
{
    unsigned char b0 : 1;
    unsigned char b1 : 1;
    unsigned char b2 : 1;
    unsigned char b3 : 1;
    unsigned char b4 : 1;
    unsigned char b5 : 1;
    unsigned char b6 : 1;
    unsigned char b7 : 1;
} _FLAG_bits;

typedef union
{
    _FLAG_bits bits;
    unsigned char byte;
} _TKS_FLAGA_type;

enum
{
    STATE_LED_OFF,
    STATE_LED_ON,
    STATE_LED_FLASH_2HZ,
    STATE_LED_FLASH_1HZ,
    STATE_LED_FLASH_0_5HZ,
    STATE_LED_FLASH_1_T,  //闪烁一下
    STATE_LED_FLASH_2_T,  //闪烁两下
    STATE_LED_FLASH_3_T,  //闪烁三下
};

enum
{
    CMD_IDEL,
    CMD_KEY,
    CMD_LED,
    CMD_REG_UP,
    CMD_REG_DOWN,
};

extern volatile _TKS_FLAGA_type bitFlag;
#define flag10ms bitFlag.bits.b0
#define flag63ms bitFlag.bits.b1
#define flag250ms bitFlag.bits.b2
#define flag500ms bitFlag.bits.b3

#endif