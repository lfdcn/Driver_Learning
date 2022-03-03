#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/input.h>

int count_event = 0;

static struct timer_list tm;
struct input_dev *vinput;

void callback(struct timer_list *timer)
{
    tm.expires = jiffies + 2 * HZ;
    add_timer(&tm); //重新开始计时

    if (count_event % 3)
    {
        input_event(vinput, EV_KEY, KEY_POWER, 0);
        input_sync(vinput);
        printk("Sendevent!sendevent0\n");
    }
    else
    {
        input_event(vinput, EV_KEY, KEY_POWER, 1);
        input_sync(vinput);
        printk("Sendevent!sendevent1\n");
    }

    count_event++;
}

static int __init vintput_init(void)
{
    int ret;

    printk("%s : %s : %d - ok.\n", __FILE__, __func__, __LINE__);

    //定时器
    timer_setup(&tm, callback, 0);
    tm.function = callback;        //指定定时时间到后的回调函数
    tm.expires = jiffies + 1 * HZ; //定时时间
    add_timer(&tm);                //注册定时器

    // input子设备
    vinput = input_allocate_device();
    if (vinput == NULL)
    {
        printk(KERN_ERR "input_allocate_device error\n");
        return -ENOMEM;
    }
    else
    {
        printk("input allocate success!\n");
    }

    //初始化input device对象
    __set_bit(EV_KEY, vinput->evbit);
    __set_bit(EV_ABS, vinput->evbit);
    __set_bit(BTN_TOUCH, vinput->keybit);

    input_set_abs_params(ft5x06.input, ABS_X, 0, 1079, 0, 0);
    input_set_abs_params(ft5x06.input, ABS_Y, 0, 2338, 0, 0);
    input_set_abs_params(ft5x06.input, ABS_MT_POSITION_X, 0, 1079, 0, 0);
    input_set_abs_params(ft5x06.input, ABS_MT_POSITION_Y, 0, 2338, 0, 0);
    //注册input device对象
    ret = input_register_device(vinput);
    if (ret != 0)
    {
        printk(KERN_ERR "input_register_device error\n");
        goto err_0;
    }
    else
    {
        printk("input register success!\n");
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
    printk("%s : %s : %d - ok.\n", __FILE__, __func__, __LINE__);

    input_unregister_device(vinput);
    input_free_device(vinput);
    printk("unregister vinput!\n");
}

module_init(vintput_init);
module_exit(vintput_exit);
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Virtual Device Code");