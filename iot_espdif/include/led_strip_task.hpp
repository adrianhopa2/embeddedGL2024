#ifndef _LED_STRIP_TASK_H
#define _LED_STRIP_TASK_H

#include "led_strip_base.hpp"

typedef void (*led_fill_rmt_items_fn)(struct led_color_t *led_strip_buf, rmt_item32_t *rmt_items, uint32_t led_strip_length);

void led_strip_task(void *arg);

#endif