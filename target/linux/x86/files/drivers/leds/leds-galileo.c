// SPDX-License-Identifier: GPL-2.0-only
/* Intel Galileo Gen 2 Arduino D13 / "L" user LED. */

#include <linux/dmi.h>
#include <linux/gpio/consumer.h>
#include <linux/gpio/machine.h>
#include <linux/leds.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

struct galileo_led {
	struct led_classdev cdev;
	struct gpio_desc *data;
	struct gpio_desc *oe;
};

/* Controller labels and offsets, never dynamic Linux GPIO numbers. */
static struct gpiod_lookup_table galileo_led_gpios = {
	.dev_id = "galileo-led",
	.table = {
		GPIO_LOOKUP("sch_gpio.2398", 7, "led", GPIO_ACTIVE_HIGH),
		GPIO_LOOKUP("i2c-INT3491:00", 14, "oe", GPIO_ACTIVE_HIGH),
		GPIO_LOOKUP("i2c-INT3491:00", 15, "pullup", GPIO_ACTIVE_HIGH),
		GPIO_LOOKUP("i2c-INT3491:01", 14, "mux", GPIO_ACTIVE_HIGH),
		{ }
	},
};

static int galileo_led_set(struct led_classdev *cdev,
			   enum led_brightness brightness)
{
	struct galileo_led *led = container_of(cdev, struct galileo_led, cdev);

	gpiod_set_value_cansleep(led->data, brightness != LED_OFF);
	return 0;
}

static void galileo_led_disable(void *data)
{
	struct galileo_led *led = data;

	gpiod_set_value_cansleep(led->oe, 1);
}

static int galileo_led_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct gpio_desc *pullup, *mux;
	struct galileo_led *led;
	int ret;

	led = devm_kzalloc(dev, sizeof(*led), GFP_KERNEL);
	if (!led)
		return -ENOMEM;

	/* Disable the buffer until its data and mux inputs are configured. */
	led->oe = devm_gpiod_get(dev, "oe", GPIOD_OUT_HIGH);
	if (IS_ERR(led->oe))
		return dev_err_probe(dev, PTR_ERR(led->oe), "D13 output enable\n");

	pullup = devm_gpiod_get(dev, "pullup", GPIOD_IN);
	if (IS_ERR(pullup))
		return dev_err_probe(dev, PTR_ERR(pullup), "D13 pullup\n");

	mux = devm_gpiod_get(dev, "mux", GPIOD_OUT_LOW);
	if (IS_ERR(mux))
		return dev_err_probe(dev, PTR_ERR(mux), "D13 GPIO mux\n");

	led->data = devm_gpiod_get(dev, "led", GPIOD_OUT_LOW);
	if (IS_ERR(led->data))
		return dev_err_probe(dev, PTR_ERR(led->data), "D13 LED GPIO\n");

	ret = devm_add_action_or_reset(dev, galileo_led_disable, led);
	if (ret)
		return ret;
	gpiod_set_value_cansleep(led->oe, 0);

	led->cdev.name = "galileo:green:user";
	led->cdev.max_brightness = 1;
	led->cdev.brightness_set_blocking = galileo_led_set;
	return devm_led_classdev_register(dev, &led->cdev);
}

static struct platform_driver galileo_led_driver = {
	.probe = galileo_led_probe,
	.driver = {
		.name = "galileo-led",
	},
};

static struct platform_device *galileo_led_device;

static int __init galileo_led_init(void)
{
	int ret;

	if (!dmi_match(DMI_BOARD_NAME, "GalileoGen2"))
		return -ENODEV;

	gpiod_add_lookup_table(&galileo_led_gpios);
	ret = platform_driver_register(&galileo_led_driver);
	if (ret)
		goto remove_lookup;

	galileo_led_device = platform_device_register_simple("galileo-led", -1,
							    NULL, 0);
	if (IS_ERR(galileo_led_device)) {
		ret = PTR_ERR(galileo_led_device);
		platform_driver_unregister(&galileo_led_driver);
		goto remove_lookup;
	}
	return 0;

remove_lookup:
	gpiod_remove_lookup_table(&galileo_led_gpios);
	return ret;
}
module_init(galileo_led_init);

static void __exit galileo_led_exit(void)
{
	platform_device_unregister(galileo_led_device);
	platform_driver_unregister(&galileo_led_driver);
	gpiod_remove_lookup_table(&galileo_led_gpios);
}
module_exit(galileo_led_exit);

MODULE_DESCRIPTION("Intel Galileo Gen 2 user LED");
MODULE_LICENSE("GPL");
