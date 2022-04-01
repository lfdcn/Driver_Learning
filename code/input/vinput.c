#include <linux/init.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>

#define MAX_SUPPORT_POINTS 5

int count_event = 0;

static struct timer_list tm;
struct input_dev *vinput;

void callback(struct timer_list *timer)
{
	tm.expires = jiffies + 2 * HZ;
	add_timer(&tm); //重新开始计时

	if (count_event % 3) {
		input_mt_slot(vinput, 1);
		input_mt_report_slot_state(vinput, MT_TOOL_FINGER, 1);
		input_report_key(vinput, BTN_TOUCH, 1);
		input_report_key(vinput, BTN_TOOL_FINGER, 1);
		input_report_abs(vinput, ABS_MT_POSITION_X, 500);
		input_report_abs(vinput, ABS_MT_POSITION_Y, 575);
		input_report_key(vinput, BTN_TOUCH, 0);
		input_report_key(vinput, BTN_TOOL_FINGER, 0);
		input_mt_sync(vinput);
		printk(KERN_WARNING "Sendevent!sendevent0\n");
	} else {
		input_mt_slot(vinput, 1);
		input_mt_report_slot_state(vinput, MT_TOOL_FINGER, 1);
		input_report_key(vinput, BTN_TOUCH, 1);
		input_report_key(vinput, BTN_TOOL_FINGER, 1);
		input_report_abs(vinput, ABS_MT_POSITION_X, 250);
		input_report_abs(vinput, ABS_MT_POSITION_Y, 300);
		input_report_key(vinput, BTN_TOUCH, 0);
		input_report_key(vinput, BTN_TOOL_FINGER, 0);
		input_mt_sync(vinput);
		printk(KERN_WARNING "Sendevent!sendevent1\n");
	}

	count_event++;
}

static int __init vintput_init(void)
{
	int ret;

	printk(KERN_WARNING "%s : %s : %d - ok.\n", __FILE__, __func__,
	       __LINE__);

	//定时器
	timer_setup(&tm, callback, 0);
	tm.function = callback; //指定定时时间到后的回调函数
	tm.expires = jiffies + 1 * HZ; //定时时间
	add_timer(&tm); //注册定时器

	// input子设备
	vinput = input_allocate_device();
	if (vinput == NULL) {
		printk(KERN_ERR "input_allocate_device error\n");
		return -ENOMEM;
	} else {
		printk(KERN_WARNING "input allocate success!\n");
	}

	//初始化input device对象
	__set_bit(EV_KEY, vinput->evbit);
	__set_bit(EV_ABS, vinput->evbit);
	__set_bit(BTN_TOUCH, vinput->keybit);
	vinput->name = "vinput_device";

	input_set_abs_params(vinput, ABS_X, 0, 1079, 0, 0);
	input_set_abs_params(vinput, ABS_Y, 0, 2339, 0, 0);
	input_set_abs_params(vinput, ABS_MT_POSITION_X, 0, 1079, 0, 0);
	input_set_abs_params(vinput, ABS_MT_POSITION_Y, 0, 2339, 0, 0);
	ret = input_mt_init_slots(vinput, MAX_SUPPORT_POINTS, 0);

	//注册input device对象
	ret = input_register_device(vinput);
	if (ret != 0) {
		printk(KERN_ERR "input_register_device error\n");
		goto err_0;
	} else {
		printk(KERN_WARNING "input register success!\n");
	}

	return 0;
err_0:
	input_free_device(vinput);
	return ret;

	return 0;
}

static void __exit vintput_exit(void)
{
	del_timer(&tm); //注销定时器
	printk(KERN_WARNING "%s : %s : %d - ok.\n", __FILE__, __func__,
	       __LINE__);

	input_unregister_device(vinput);
	input_free_device(vinput);
	printk(KERN_WARNING "unregister vinput!\n");
}

module_init(vintput_init);
module_exit(vintput_exit);
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Virtual Device Code");
