#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "protobuf/protobuf_message.hpp"
#include "storage/non_volatile_storage.hpp"

#include "protobuf/storage.pb.h"
#include <pb_decode.h>
#include <pb_encode.h>

LOG_MODULE_REGISTER(example, LOG_LEVEL_DBG);

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

void handle_boot_counter()
{
	non_volatile_storage storage{};
	const auto init_error = storage.init();
	if (init_error) {
		LOG_ERR("Failed to initialize the storage: %s", init_error.message());
	}

	constexpr uint16_t static_number_id = 2U;

	const auto number = storage.read<uint16_t>(static_number_id);
	if (!number) {
		LOG_ERR("Failed to read number: %s", number.error().message());
	} else {
		LOG_DBG("Read static number: %d", number.value());
	}

	const auto write_number_error = storage.write<uint16_t>(static_number_id, 42U);
	if (write_number_error) {
		LOG_ERR("Failed to write static number: %s", write_number_error.message());
	}

	protobuf::message<RuntimeStatistics, RuntimeStatistics_size> message{RuntimeStatistics_msg};

	constexpr uint16_t reboot_counter_id = 1U;

	const auto read_error = storage.read(reboot_counter_id, message);
	if (read_error) {
		LOG_ERR("Failed to read: %s", read_error.message());
	}

	LOG_INF("Reboot counter: %d", message.data().boot_count);

	message.data().boot_count++;

	const auto write_error = storage.write(reboot_counter_id, message);
	if (write_error) {
		LOG_ERR("Failed to write: %s", write_error.message());
	}
}

void led_blink_loop()
{
	if (!gpio_is_ready_dt(&led)) {
		return;
	}

	int ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}

	while (1) {
		ret = gpio_pin_toggle_dt(&led);
		if (ret < 0) {
			return;
		}
		os::sleep(1s);
	}
}

int main(void)
{
	LOG_DBG("Starting main function.");
	handle_boot_counter();
	led_blink_loop();
	return 0;
}
