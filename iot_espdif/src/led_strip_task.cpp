#include "led_strip_task.hpp"

void led_strip_task(void *arg)
{
    LedStrip *strip = static_cast<LedStrip *>(arg);
    struct led_strip_t *led_strip = strip->getStrip();

    bool make_new_rmt_items = true;
    bool prev_showing_buf_1 = !led_strip->showing_buf_1;

    size_t num_items_malloc = (LED_STRIP_NUM_RMT_ITEMS_PER_LED * led_strip->led_strip_length);
    rmt_item32_t *rmt_items = (rmt_item32_t *)malloc(sizeof(rmt_item32_t) * num_items_malloc);
    if (!rmt_items)
    {
        vTaskDelete(NULL);
    }

    RmtWrapper rmtwrapper;

    for (;;)
    {
        //rmt_wait_tx_done(led_strip->rmt_channel, portMAX_DELAY);
        rmtwrapper.wait_tx_done(led_strip->rmt_channel, portMAX_DELAY);
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
            strip->fill_rmt_items(rmt_items);
        }

        //rmt_write_items(led_strip->rmt_channel, rmt_items, num_items_malloc, false);
        rmtwrapper.write_items(led_strip->rmt_channel, rmt_items, num_items_malloc, false);
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