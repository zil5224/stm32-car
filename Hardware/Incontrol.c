#include "stm32f10x.h"
#include "Delay.h"

/* 红外接收头输出接在 PA8，空闲时通常为高电平。 */
#define IRED_PORT       GPIOA
#define IRED_PIN        GPIO_Pin_8
#define IRED_PORT_RCC   RCC_APB2Periph_GPIOA

/*
 * 驱动层在校验通过后保存整帧并置位标志，应用层处理该帧时清除标志。
 */
uint32_t IR_Receivecode;
uint8_t  IR_Receiveflag;

/*
 * 每次轮询延时 20 微秒。返回的是轮询次数，实际脉宽还包含读 GPIO 等开销。
 * 电平一直未翻转时，计数到 max_ticks 就结束，调用方可据此丢弃本帧。
 */
static uint16_t IRremote_MeasureLevel(uint8_t expected_level, uint16_t max_ticks)
{
    uint16_t elapsed_ticks = 0;

    while (GPIO_ReadInputDataBit(IRED_PORT, IRED_PIN) == expected_level)
    {
        Delay_us(20);
        elapsed_ticks++;

        if (elapsed_ticks >= max_ticks)
        {
            break;
        }
    }

    return elapsed_ticks;
}

void IRremote_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 本方案采用主循环轮询，因此这里只初始化 GPIO，不再配置 EXTI/NVIC。 */
    RCC_APB2PeriphClockCmd(IRED_PORT_RCC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = IRED_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IRED_PORT, &GPIO_InitStructure);

    IR_Receivecode = 0;
    IR_Receiveflag = 0;
}

/*
 * 轮询接收一帧 NEC 风格红外数据。
 *
 * 接收头输出的典型时序为：
 *   约 9ms 低电平 + 约 4.5ms 高电平：引导码
 *   每一位：约 560us 低电平 + 560us/1690us 高电平：0/1
 *
 * 每接收一位就左移并写入最低位。组帧后，从高位到低位依次为地址、
 * 地址反码、命令和命令反码。
 */
uint8_t IRremote_Scan(void)
{
    uint16_t low_ticks;
    uint16_t high_ticks;
    uint8_t received_bit;
    uint8_t bit_index;
    uint32_t frame = 0;
    uint8_t address;
    uint8_t address_inverse;
    uint8_t command;
    uint8_t command_inverse;

    /* 空闲状态为高电平，没有红外帧时立即返回，保证主循环保持响应。 */
    if (GPIO_ReadInputDataBit(IRED_PORT, IRED_PIN) != 0)
    {
        return 0;
    }

    /* 等待并检查约 9ms 的引导低电平。 */
    low_ticks = IRremote_MeasureLevel(0, 600);
    if (low_ticks < 300 || low_ticks >= 600)
    {
        return 0;
    }

    /*
     * 正常帧的引导高电平约 4.5ms；长按重复码约 2.25ms。
     * 重复码不携带新的 32 位数据，这里忽略它，避免重复触发动作。
     */
    high_ticks = IRremote_MeasureLevel(1, 300);
    if (high_ticks >= 80 && high_ticks < 160)
    {
        return 0;
    }
    if (high_ticks < 180 || high_ticks >= 300)
    {
        return 0;
    }

    /* 依次读取 32 位数据。NEC 的每一位都先有一段约 560us 低电平。 */
    for (bit_index = 0; bit_index < 32; bit_index++)
    {
        low_ticks = IRremote_MeasureLevel(0, 80);
        if (low_ticks < 10 || low_ticks >= 80)
        {
            return 0;
        }

        high_ticks = IRremote_MeasureLevel(1, 120);
        if (high_ticks >= 10 && high_ticks < 50)
        {
            received_bit = 0;
        }
        else if (high_ticks >= 60 && high_ticks < 110)
        {
            received_bit = 1;
        }
        else
        {
            /* 脉宽不属于 0 或 1，说明本帧时序不可靠。 */
            return 0;
        }

        frame = (frame << 1) | received_bit;
    }

    /*
     * 一组原码与反码逐位互补，异或结果应为 0xFF。
     * 地址和命令两组都通过校验后，才把这一帧交给应用层。
     */
    address = (uint8_t)(frame >> 24);
    address_inverse = (uint8_t)(frame >> 16);
    command = (uint8_t)(frame >> 8);
    command_inverse = (uint8_t)frame;

    if ((uint8_t)(address ^ address_inverse) != 0xFF ||
        (uint8_t)(command ^ command_inverse) != 0xFF)
    {
        return 0;
    }

    IR_Receivecode = frame;
    IR_Receiveflag = 1;
    return 1;
}
