#include "led_strip_base.hpp"

LedStrip::LedStrip(ILedStripDrv &driver, led_strip_t *led_strip) : ledStripDriver(driver)
{

    ledStripConfig.led_strip_length = led_strip->led_strip_length;
    ledStripConfig.rmt_channel = led_strip->rmt_channel;
    ledStripConfig.rmt_interrupt_num = led_strip->rmt_interrupt_num;
    ledStripConfig.gpio = led_strip->gpio;
    ledStripConfig.showing_buf_1 = led_strip->showing_buf_1;
    ledStripConfig.led_strip_buf_1 = led_strip->led_strip_buf_1;
    ledStripConfig.led_strip_buf_2 = led_strip->led_strip_buf_2;
    ledStripConfig.access_semaphore = led_strip->access_semaphore;

    m_led_strip_ok = true;
}

bool LedStrip::init()
{
    RmtWrapper rmtwrapper;

    if (!m_led_strip_ok ||
        (ledStripConfig.rmt_channel == RMT_CHANNEL_MAX) ||
        (ledStripConfig.gpio > GPIO_NUM_33) || // only inputs above 33
        (ledStripConfig.led_strip_buf_1 == NULL) ||
        (ledStripConfig.led_strip_buf_2 == NULL) ||
        (ledStripConfig.led_strip_length == 0) ||
        (ledStripConfig.access_semaphore == NULL))
    {
        return false;
    }

    if (ledStripConfig.led_strip_buf_1 == ledStripConfig.led_strip_buf_2)
    {
        return false;
    }

    memset(ledStripConfig.led_strip_buf_1, 0, sizeof(struct led_color_t) * ledStripConfig.led_strip_length);
    memset(ledStripConfig.led_strip_buf_2, 0, sizeof(struct led_color_t) * ledStripConfig.led_strip_length);

    rmt_config_t rmt_cfg = {
        .rmt_mode = RMT_MODE_TX,
        .channel = ledStripConfig.rmt_channel,
        .gpio_num = ledStripConfig.gpio,
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

    esp_err_t cfg_ok = rmtwrapper.config(&rmt_cfg);
    if (cfg_ok != ESP_OK)
    {
        return false;
    }

    esp_err_t install_ok = rmtwrapper.driver_install(rmt_cfg.channel, 0, 0);
    if (install_ok != ESP_OK)
    {
        return false;
    }

    xSemaphoreGive(ledStripConfig.access_semaphore);

    return true;
}

bool LedStrip::set_pixel_rgb(uint32_t pixel_num, uint8_t red, uint8_t green, uint8_t blue)
{
    bool set_led_success = true;

    if (!m_led_strip_ok || (pixel_num > ledStripConfig.led_strip_length))
    {
        return false;
    }

    if (ledStripConfig.showing_buf_1)
    {
        ledStripConfig.led_strip_buf_2[pixel_num].red = red;
        ledStripConfig.led_strip_buf_2[pixel_num].green = green;
        ledStripConfig.led_strip_buf_2[pixel_num].blue = blue;
    }
    else
    {
        ledStripConfig.led_strip_buf_1[pixel_num].red = red;
        ledStripConfig.led_strip_buf_1[pixel_num].green = green;
        ledStripConfig.led_strip_buf_1[pixel_num].blue = blue;
    }

    return set_led_success;
}

bool LedStrip::show()
{
    bool success = true;

    if (!m_led_strip_ok)
    {
        return false;
    }

    xSemaphoreTake(ledStripConfig.access_semaphore, portMAX_DELAY);
    if (ledStripConfig.showing_buf_1)
    {
        ledStripConfig.showing_buf_1 = false;
        memset(ledStripConfig.led_strip_buf_1, 0, sizeof(struct led_color_t) * ledStripConfig.led_strip_length);
    }
    else
    {
        ledStripConfig.showing_buf_1 = true;
        memset(ledStripConfig.led_strip_buf_2, 0, sizeof(struct led_color_t) * ledStripConfig.led_strip_length);
    }
    xSemaphoreGive(ledStripConfig.access_semaphore);

    return success;
}

bool LedStrip::clear()
{
    bool success = true;

    if (!m_led_strip_ok)
    {
        return false;
    }

    if (ledStripConfig.showing_buf_1)
    {
        memset(ledStripConfig.led_strip_buf_2, 0, sizeof(struct led_color_t) * ledStripConfig.led_strip_length);
    }
    else
    {
        memset(ledStripConfig.led_strip_buf_1, 0, sizeof(struct led_color_t) * ledStripConfig.led_strip_length);
    }

    return success;
}

led_strip_t *LedStrip::getStrip()
{
    return &ledStripConfig;
}

void LedStrip::fill_rmt_items(rmt_item32_t *rmt_items)
{
    if (ledStripConfig.showing_buf_1)
    {
        ledStripDriver.fill_rmt_items(ledStripConfig.led_strip_buf_1, rmt_items, ledStripConfig.led_strip_length);
    }
    else
    {
        ledStripDriver.fill_rmt_items(ledStripConfig.led_strip_buf_2, rmt_items, ledStripConfig.led_strip_length);
    }
}
