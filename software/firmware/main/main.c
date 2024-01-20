/* KSoft Posture Corrector - KSPC
 */
#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_sleep.h"
#include "i2c.h"
#include "adxl345.h"

static const char *TAG = "KSPC";
#define GPIO_VIB_MOTOR 3
#define GPIO_OUTPUT_PIN_SEL ((1ULL << GPIO_VIB_MOTOR))

/**
 * @brief i2c master initialization
 */
static esp_err_t gpio_init()
{
    // zero-initialize the config structure.
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    return gpio_config(&io_conf);
}

void start_deep_sleep(void)
{
    esp_sleep_enable_timer_wakeup(25 * 1000000);
    esp_deep_sleep_start();
}

void app_main(void)
{
    ESP_ERROR_CHECK(gpio_init());

    i2c_init(I2C_CONTROLLER_0, 6, 7);
    adxl345_init(I2C_CONTROLLER_0);

    while (true)
    {
        for (size_t i = 0; i < 10; i++)
        {
            int z = adxl345_read_z(I2C_CONTROLLER_0);

            if (z < -155)
            {
                gpio_set_level(GPIO_VIB_MOTOR, 1);
                vTaskDelay(500 / portTICK_PERIOD_MS);
                gpio_set_level(GPIO_VIB_MOTOR, 0);
                continue;
            }
            else if (z > 40)
            {
                gpio_set_level(GPIO_VIB_MOTOR, 1);
                vTaskDelay(500 / portTICK_PERIOD_MS);
                gpio_set_level(GPIO_VIB_MOTOR, 0);
                continue;
            }
            else
            {
                gpio_set_level(GPIO_VIB_MOTOR, 0);
            }

            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
        start_deep_sleep();
        //break;
    }
}
