#include <linux/delay.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/timer.h>

#define FINGERS 3
#define finger_index FINGERS + 1

struct input_data
{
	struct timer_list tm;
	struct input_dev *vinput;
	unsigned char finger;
};