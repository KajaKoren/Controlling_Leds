/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/drivers/led.h>

#include "message_channel.h"

/* Register log module */
LOG_MODULE_REGISTER(led, 4);

const static struct device *led_device = DEVICE_DT_GET_ANY(gpio_leds);

/* LED 1, green on Thingy:91 boards. */
#define LED_1_GREEN 1
/*static uint8_t payload_buf[CONFIG_MQTT_PAYLOAD_BUFFER_SIZE];*/
struct payload payload;

void led_callback(const struct zbus_channel *chan)
{
	int err = 0;
	const enum network_status *status;

	if (&NETWORK_CHAN == chan) {

		if (!device_is_ready(led_device)) {
			LOG_ERR("LED device is not ready");
			return;
		}

		/* Get network status from channel. */
		status = zbus_chan_const_msg(chan);

		switch (*status) {
		case NETWORK_CONNECTED:
			err = led_on(led_device, LED_1_GREEN);
			if (err) {
				LOG_ERR("led_on, error: %d", err);
			}
			LOG_DBG("NETWORK CONNECTED");
			break;
		case NETWORK_DISCONNECTED:
			err = led_off(led_device, LED_1_GREEN);
			if (err) {
				LOG_ERR("led_off, error: %d", err);
			}
			break;
		default:
			LOG_ERR("Unknown event: %d", *status);
			break;
		}
	}

	if(&PAYLOAD_CHAN == chan){
		const struct payload payload;
		payload = zbus_chan_const_msg(&PAYLOAD_CHAN, &payload, K_SECONDS(1));
			if (err) {
				LOG_ERR("zbus_chan_read, error: %d", err);
				SEND_FATAL_ERROR();
				return;
			}

		
		LOG_DBG("payload %s", payload.string);

		if(strncmp(payload.string,CONFIG_TURN_LED_ON_CMD,sizeof(CONFIG_TURN_LED_ON_CMD)-1) == 0){
				led_on(led_device, LED_1_GREEN);
			}
		else if(strncmp(payload.string,CONFIG_TURN_LED_OFF_CMD,sizeof(CONFIG_TURN_LED_OFF_CMD)-1) == 0){
			led_off(led_device, LED_1_GREEN);
		}
	}

}

/* Register listener - led_callback will be called everytime a channel that the module listens on
 * receives a new message.
 */
ZBUS_LISTENER_DEFINE(led, led_callback);
