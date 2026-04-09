/** @brief      spi to led demo.
 *
 *  @copyright  Copyright (C) 2025, Shenzhen Anxinke Technology Co., Ltd
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "hosal_spi.h"
#include "bl602_spi.h"
#include "bl602_glb.h"
#include "bl602_pwm.h"
#include "bl_irq.h"
#include "bl_dma.h"
#include "bl602_dma.h"
#include "hosal_dma.h"

#define LED_NUM     8
#define LED_T0      0xC0
#define LED_T1      0xFC

#define DEMO_SPI_ID      0
#define DEMO_SPI_MOSI    GLB_GPIO_PIN_12
#define DEMO_SPI_CLK_HZ  6666666
#define DEMO_DMA_LLI_CNT 2

static uint8_t led_buffer[3 * 8 * LED_NUM];
static volatile int spi_dma_txing;
static hosal_dma_chan_t spi_dma_chan;
static DMA_LLI_Ctrl_Type spi_dma_lli[DEMO_DMA_LLI_CNT];

static void spi_dma_int_handler(void *arg, uint32_t flag)
{
    // bl_dma_int_clear(spi_dma_chan);
    // puts("spi_dma_int_handler\r\n");

    spi_dma_txing = 0;

    return;
}

static void inline spi_dma_whait_txdone(void)
{
    while (spi_dma_txing) {
        ;
    }
}

static int spi_dma_lli_list_init(uint8_t *data, uint32_t length)
{
    struct DMA_Control_Reg dmactrl;

    dmactrl.SBSize = DMA_BURST_SIZE_1;
    dmactrl.DBSize = DMA_BURST_SIZE_1;
    dmactrl.SWidth = DMA_TRNS_WIDTH_8BITS;
    dmactrl.DWidth = DMA_TRNS_WIDTH_8BITS;
    dmactrl.Prot = 0;
    dmactrl.SLargerD = 0;

    dmactrl.TransferSize = length;
    dmactrl.I = 0;

    dmactrl.SI = DMA_MINC_ENABLE;
    dmactrl.DI = DMA_MINC_DISABLE;

    spi_dma_lli[0].srcDmaAddr = (uint32_t)(data);
    spi_dma_lli[0].destDmaAddr = (uint32_t)(SPI_BASE + SPI_FIFO_WDATA_OFFSET);
    spi_dma_lli[0].dmaCtrl = dmactrl;
    spi_dma_lli[0].nextLLI = 0;

    return 0;
}

static int spi_send_by_dma(uint8_t *data, uint32_t length)
{
    DMA_LLI_Cfg_Type txllicfg;

    // if (length > LLI_BUFF_SIZE)
    //     assert(0);

    txllicfg.dir = DMA_TRNS_M2P;
    txllicfg.srcPeriph = DMA_REQ_NONE; 
    txllicfg.dstPeriph = DMA_REQ_SPI_TX;

    spi_dma_lli_list_init(data, length);

    DMA_LLI_Init(spi_dma_chan, &txllicfg);
    DMA_LLI_Update(spi_dma_chan, (uint32_t)spi_dma_lli);
    hosal_dma_irq_callback_set(spi_dma_chan, spi_dma_int_handler, NULL);
    hosal_dma_chan_start(spi_dma_chan);

    return 0;
}

static void spi_gpio_init(void)
{
    GLB_GPIO_Type pin_mosi;

    pin_mosi = DEMO_SPI_MOSI;
    GLB_GPIO_Func_Init(GPIO_FUN_SPI, &pin_mosi, 1);

    GLB_Set_SPI_0_ACT_MOD_Sel(GLB_SPI_PAD_ACT_AS_MASTER);

    return;
}

static void led_spi_init(void)
{
    SPI_CFG_Type spiCfg = {
        DISABLE,                      /* De-glitch function */
        ENABLE,                       /* Master continuous transfer mode */
        SPI_BYTE_INVERSE_BYTE0_FIRST, /* The byte 0 is sent first in SPI transfer */
        SPI_BIT_INVERSE_MSB_FIRST,    /* MSB is sent first in SPI transfer */
        SPI_CLK_PHASE_INVERSE_0,      /* SPI clock phase */
        SPI_CLK_POLARITY_LOW,         /* SPI clock plarity */
        SPI_FRAME_SIZE_8
    };

    SPI_FifoCfg_Type fifoCfg = {
        1,      /* SPI tx FIFO threshold */
        0,      /* SPI rx FIFO threshold */
        ENABLE, /* Enable or disable tx dma req/ack interface */
        DISABLE /* Enable or disable rx dma req/ack interface */
    };

    spi_dma_chan = hosal_dma_chan_request(0);

    SPI_Disable(DEMO_SPI_ID, SPI_WORK_MODE_MASTER);
    // SPI_IntMask(DEMO_SPI_ID, SPI_INT_ALL, MASK);
    SPI_Init(DEMO_SPI_ID, &spiCfg);
    SPI_FifoConfig(DEMO_SPI_ID, &fifoCfg);
    SPI_SetClock(DEMO_SPI_ID, DEMO_SPI_CLK_HZ);
    SPI_Enable(DEMO_SPI_ID, SPI_WORK_MODE_MASTER);
}

static void led_conv_buff(int32_t led_id, uint8_t r, uint8_t g, uint8_t b)
{
    for (int32_t i = 0; i < 8; i++) {
        led_buffer[led_id * 24 + i + 0] = ((g >> (8 - i)) & 0x01) ? LED_T1 : LED_T0;
    }
    for (int32_t i = 0; i < 8; i++) {
        led_buffer[led_id * 24 + i + 8] = ((r >> (8 - i)) & 0x01) ? LED_T1 : LED_T0;
    }
    for (int32_t i = 0; i < 8; i++) {
        led_buffer[led_id * 24 + i + 16] = ((b >> (8 - i)) & 0x01) ? LED_T1 : LED_T0;
    }
}

static void rgb_table_deal(void)
{
#define BRIGHT_MAX 120
#define BRIGHT_MIN 5
#define BRIGHT_SETP 1
    static uint8_t level;
    static int8_t direction = BRIGHT_SETP;

    for (int i = 0; i < LED_NUM; i++) {
        led_conv_buff(i, level, level, (130 - level) % 100);
    }

    if (level > BRIGHT_MAX) {
        direction = -BRIGHT_SETP;
    } else if (level < BRIGHT_MIN) {
        direction = BRIGHT_SETP;
    }
    level += direction;
}

int main(void)
{
    spi_gpio_init();
    led_spi_init();

    printf("demo task start...\r\n");
    while (1)
    {
        spi_dma_whait_txdone();
        rgb_table_deal();

        // SPI_Send_8bits(0, led_buffer, sizeof led_buffer, SPI_TIMEOUT_DISABLE);
        spi_send_by_dma(led_buffer, sizeof led_buffer);

        vTaskDelay(10);
    }
}