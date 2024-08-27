#include "led_ws2812b_drv.hpp"

static inline void led_strip_fill_item_level(rmt_item32_t *item, int high_ticks, int low_ticks)
{
    item->level0 = 1;
    item->duration0 = high_ticks;
    item->level1 = 0;
    item->duration1 = low_ticks;
}

static void led_strip_fill_rmt_items_ws2812(led_color_t *led_strip_buf, rmt_item32_t *rmt_items, uint32_t led_strip_length)
{
    uint32_t rmt_items_index = 0;
    for (uint32_t led_index = 0; led_index < led_strip_length; led_index++)
    {
        struct led_color_t led_color = led_strip_buf[led_index];

        for (uint8_t bit = 8; bit != 0; bit--)
        {
            uint8_t bit_set = (led_color.green >> (bit - 1)) & 1;
            if (bit_set)
            {
                led_strip_fill_item_level((&(rmt_items[rmt_items_index])), LED_STRIP_RMT_TICKS_BIT_1_HIGH_WS2812, LED_STRIP_RMT_TICKS_BIT_1_LOW_WS2812);
            }
            else
            {
                led_strip_fill_item_level((&(rmt_items[rmt_items_index])), LED_STRIP_RMT_TICKS_BIT_0_HIGH_WS2812, LED_STRIP_RMT_TICKS_BIT_0_LOW_WS2812);
            }
            rmt_items_index++;
        }
        for (uint8_t bit = 8; bit != 0; bit--)
        {
            uint8_t bit_set = (led_color.red >> (bit - 1)) & 1;
            if (bit_set)
            {
                led_strip_fill_item_level((&(rmt_items[rmt_items_index])), LED_STRIP_RMT_TICKS_BIT_1_HIGH_WS2812, LED_STRIP_RMT_TICKS_BIT_1_LOW_WS2812);
            }
            else
            {
                led_strip_fill_item_level((&(rmt_items[rmt_items_index])), LED_STRIP_RMT_TICKS_BIT_0_HIGH_WS2812, LED_STRIP_RMT_TICKS_BIT_0_LOW_WS2812);
            }
            rmt_items_index++;
        }
        for (uint8_t bit = 8; bit != 0; bit--)
        {
            uint8_t bit_set = (led_color.blue >> (bit - 1)) & 1;
            if (bit_set)
            {
                led_strip_fill_item_level((&(rmt_items[rmt_items_index])), LED_STRIP_RMT_TICKS_BIT_1_HIGH_WS2812, LED_STRIP_RMT_TICKS_BIT_1_LOW_WS2812);
            }
            else
            {
                led_strip_fill_item_level((&(rmt_items[rmt_items_index])), LED_STRIP_RMT_TICKS_BIT_0_HIGH_WS2812, LED_STRIP_RMT_TICKS_BIT_0_LOW_WS2812);
            }
            rmt_items_index++;
        }
    }
}

static void led_strip_task(void *arg)
{
    struct led_strip_t *led_strip = (struct led_strip_t *)arg;
    led_fill_rmt_items_fn led_make_waveform = NULL;
    bool make_new_rmt_items = true;
    bool prev_showing_buf_1 = !led_strip->showing_buf_1;

    size_t num_items_malloc = (LED_STRIP_NUM_RMT_ITEMS_PER_LED * led_strip->led_strip_length);
    rmt_item32_t *rmt_items = (rmt_item32_t *)malloc(sizeof(rmt_item32_t) * num_items_malloc);
    if (!rmt_items)
    {
        vTaskDelete(NULL);
    }

    switch (led_strip->rgb_led_type)
    {
    case RGB_LED_TYPE_WS2812:
        led_make_waveform = led_strip_fill_rmt_items_ws2812;
        break;

    default:
        // Will avoid keeping it point to NULL
        led_make_waveform = led_strip_fill_rmt_items_ws2812;
        break;
    };

    for (;;)
    {
        rmt_wait_tx_done(led_strip->rmt_channel, portMAX_DELAY);
        xSemaphoreTake(led_strip->access_semaphore, portMAX_DELAY);

        /*
         * If buf 1 was previously being shown and now buf 2 is being shown,
         * it should update the new rmt items array. If buf 2 was previous being shown
         * and now buf 1 is being shown, it should update the new rmt items array.
         * Otherwise, no need to update the array
         */
        if ((prev_showing_buf_1 == true) && (led_strip->showing_buf_1 == false))
        {
            make_new_rmt_items = true;
        }
        else if ((prev_showing_buf_1 == false) && (led_strip->showing_buf_1 == true))
        {
            make_new_rmt_items = true;
        }
        else
        {
            make_new_rmt_items = false;
        }

        if (make_new_rmt_items)
        {
            if (led_strip->showing_buf_1)
            {
                led_make_waveform(led_strip->led_strip_buf_1, rmt_items, led_strip->led_strip_length);
            }
            else
            {
                led_make_waveform(led_strip->led_strip_buf_2, rmt_items, led_strip->led_strip_length);
            }
        }

        rmt_write_items(led_strip->rmt_channel, rmt_items, num_items_malloc, false);
        prev_showing_buf_1 = led_strip->showing_buf_1;
        xSemaphoreGive(led_strip->access_semaphore);
        vTaskDelay(LED_STRIP_REFRESH_PERIOD_MS / portTICK_PERIOD_MS);
    }

    if (rmt_items)
    {
        free(rmt_items);
    }
    vTaskDelete(NULL);
}

LedWS2812BDrv::LedWS2812BDrv(led_strip_t *led_strip)
{
    m_led_strip.rgb_led_type = led_strip->rgb_led_type;
    m_led_strip.led_strip_length = led_strip->led_strip_length;
    m_led_strip.rmt_channel = led_strip->rmt_channel;
    m_led_strip.rmt_interrupt_num = led_strip->rmt_interrupt_num;
    m_led_strip.gpio = led_strip->gpio;
    m_led_strip.showing_buf_1 = led_strip->showing_buf_1;
    m_led_strip.led_strip_buf_1 = led_strip->led_strip_buf_1;
    m_led_strip.led_strip_buf_2 = led_strip->led_strip_buf_2;
    m_led_strip.access_semaphore = led_strip->access_semaphore;

    m_led_strip_ok = true;
}

bool LedWS2812BDrv::led_strip_init()
{
    TaskHandle_t led_strip_task_handle;

    if (!m_led_strip_ok ||
        (m_led_strip.rmt_channel == RMT_CHANNEL_MAX) ||
        (m_led_strip.gpio > GPIO_NUM_33) || // only inputs above 33
        (m_led_strip.led_strip_buf_1 == NULL) ||
        (m_led_strip.led_strip_buf_2 == NULL) ||
        (m_led_strip.led_strip_length == 0) ||
        (m_led_strip.access_semaphore == NULL))
    {
        return false;
    }

    if (m_led_strip.led_strip_buf_1 == m_led_strip.led_strip_buf_2)
    {
        return false;
    }

    memset(m_led_strip.led_strip_buf_1, 0, sizeof(struct led_color_t) * m_led_strip.led_strip_length);
    memset(m_led_strip.led_strip_buf_2, 0, sizeof(struct led_color_t) * m_led_strip.led_strip_length);

    rmt_config_t rmt_cfg = {
        .rmt_mode = RMT_MODE_TX,
        .channel = m_led_strip.rmt_channel,
        .gpio_num = m_led_strip.gpio,
        .clk_div = LED_STRIP_RMT_CLK_DIV,
        .mem_block_num = 1,
        .tx_config = {
            .carrier_freq_hz = 100, // Not used, but has to be set to avoid divide by 0 err
            .carrier_level = RMT_CARRIER_LEVEL_LOW,
            .idle_level = RMT_IDLE_LEVEL_LOW,
            .carrier_duty_percent = 50,
            .carrier_en = false,
            .loop_en = false,
            .idle_output_en = true,
        }};

    esp_err_t cfg_ok = rmt_config(&rmt_cfg);
    if (cfg_ok != ESP_OK)
    {
        return false;
    }
    esp_err_t install_ok = rmt_driver_install(rmt_cfg.channel, 0, 0);
    if (install_ok != ESP_OK)
    {
        return false;
    }

    xSemaphoreGive(m_led_strip.access_semaphore);
    BaseType_t task_created = xTaskCreate(led_strip_task,
                                          "led_strip_task",
                                          LED_STRIP_TASK_SIZE,
                                          &m_led_strip,
                                          LED_STRIP_TASK_PRIORITY,
                                          &led_strip_task_handle);

    if (!task_created)
    {
        return false;
    }

    return true;
}

bool LedWS2812BDrv::led_strip_set_pixel_rgb(uint32_t pixel_num, uint8_t red, uint8_t green, uint8_t blue)
{
    bool set_led_success = true;

    if (!m_led_strip_ok || (pixel_num > m_led_strip.led_strip_length))
    {
        return false;
    }

    if (m_led_strip.showing_buf_1)
    {
        m_led_strip.led_strip_buf_2[pixel_num].red = red;
        m_led_strip.led_strip_buf_2[pixel_num].green = green;
        m_led_strip.led_strip_buf_2[pixel_num].blue = blue;
    }
    else
    {
        m_led_strip.led_strip_buf_1[pixel_num].red = red;
        m_led_strip.led_strip_buf_1[pixel_num].green = green;
        m_led_strip.led_strip_buf_1[pixel_num].blue = blue;
    }

    return set_led_success;
}

bool LedWS2812BDrv::led_strip_show()
{
    bool success = true;

    if (!m_led_strip_ok)
    {
        return false;
    }

    xSemaphoreTake(m_led_strip.access_semaphore, portMAX_DELAY);
    if (m_led_strip.showing_buf_1)
    {
        m_led_strip.showing_buf_1 = false;
        memset(m_led_strip.led_strip_buf_1, 0, sizeof(struct led_color_t) * m_led_strip.led_strip_length);
    }
    else
    {
        m_led_strip.showing_buf_1 = true;
        memset(m_led_strip.led_strip_buf_2, 0, sizeof(struct led_color_t) * m_led_strip.led_strip_length);
    }
    xSemaphoreGive(m_led_strip.access_semaphore);

    return success;
}

bool LedWS2812BDrv::led_strip_clear()
{
    bool success = true;

    if (!m_led_strip_ok)
    {
        return false;
    }

    if (m_led_strip.showing_buf_1)
    {
        memset(m_led_strip.led_strip_buf_2, 0, sizeof(struct led_color_t) * m_led_strip.led_strip_length);
    }
    else
    {
        memset(m_led_strip.led_strip_buf_1, 0, sizeof(struct led_color_t) * m_led_strip.led_strip_length);
    }

    return success;
}

LedWS2812BDrv::~LedWS2812BDrv()
{
}
