#include "sccb.h"

void SCCB_delay(void)
{
    volatile int16u_t time;

    for (time = 12800; time > 0; time--);
}

/*
 *
 *
 *
 */
void SCCB_start(void)
{
    // SCCB   总线输出高电平
    SCCB_CLK_HIGH();
    SCCB_CLK_OUT();

    SCCB_delay();

    SCCB_DAT_HIGH();
    SCCB_DAT_OUT();

    SCCB_delay();

    // SCCB 总线发出启动信号
    SCCB_DAT_LOW();
    SCCB_delay();

    SCCB_CLK_LOW();
    SCCB_delay();
}

/*
 *
 *
 *
 */
void SCCB_stop(void)
{
    // SCCB   总线输出低电平
    SCCB_CLK_LOW();
    SCCB_CLK_OUT();

    SCCB_delay();

    SCCB_DAT_LOW();
    SCCB_DAT_OUT();

    SCCB_delay();

    // SCCB 总线发出停止信号
    SCCB_CLK_HIGH();
    SCCB_delay();

    SCCB_DAT_HIGH();
    SCCB_delay();
}

/*
 *
 *
 *
 */
int8u_t SCCB_write_byte(int8u_t data)
{
    int8u_t test, ack;

    for (test = 0x80; test != 0; test >>= 1)
    {
        if (data & test)
        {
            SCCB_DAT_HIGH();
            SCCB_DAT_OUT();
        }

        else
        {
            SCCB_DAT_LOW();
            SCCB_DAT_OUT();
        }

        SCCB_delay();
        SCCB_CLK_HIGH();

        SCCB_delay();
        SCCB_CLK_LOW();
    }

    // SCCB 数据端口设置输入
    SCCB_DAT_IN();
    SCCB_delay();

    SCCB_CLK_HIGH();
    SCCB_delay();

    // SCCB 读取从机应答状态
    if (SCCB_DAT_RAD())
    {
        ack = SCCB_NACK;
    }

    else
    {
        ack = SCCB_ACK;
    }

    SCCB_CLK_LOW();

    return ack;
}

/*
 *
 *
 *
 */
int8u_t SCCB_read_byte(int8u_t ack)
{
    int8u_t test, data = 0;

    // SCCB 数据端口设置输入
    SCCB_DAT_IN();
    SCCB_delay();

    for (test = 0x80; test != 0; test >>= 1)
    {
        SCCB_CLK_HIGH();
        SCCB_delay();

        if (SCCB_DAT_RAD())
        {
            data |= test;
        }

        SCCB_CLK_LOW();
        SCCB_delay();
    }

    // SCCB 主机发送应答状态
    if (ack == SCCB_ACK)
    {
        SCCB_DAT_LOW();
        SCCB_DAT_OUT();
    }

    else
    {
        SCCB_DAT_HIGH();
        SCCB_DAT_OUT();
    }

    // SCCB 总线发送应答时钟
    SCCB_CLK_HIGH();
    SCCB_delay();

    SCCB_CLK_LOW();

    return data;
}
