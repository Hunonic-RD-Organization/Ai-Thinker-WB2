#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>

#include <bl602_spi.h>
#include <bl_gpio.h>
#include <blog.h>
#include <hosal_dma.h>

int main(void)
{

    for (;;)
    {
        vTaskDelay(portTICK_RATE_MS * 10000);
    }

    return 0;
}
