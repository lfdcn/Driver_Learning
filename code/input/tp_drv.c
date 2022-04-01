#include "vinput_timer.h"

int count_event = 0;
struct input_data *input_data_mid;

void report_data(/*unsigned char finger,*/ unsigned int finger_num)
{
    input_mt_slot(input_data_mid->vinput, 0);
    input_mt_report_slot_state(input_data_mid->vinput, MT_TOOL_FINGER, 1);

    input_report_key(input_data_mid->vinput, BTN_TOUCH, 1);
    // input_report_key(input_data_mid->vinput, BTN_TOOL_FINGER, 1);

    input_report_abs(input_data_mid->vinput, ABS_MT_POSITION_X, finger_num);
    input_report_abs(input_data_mid->vinput, ABS_MT_POSITION_Y, finger_num);
    input_sync(input_data_mid->vinput);
    mdelay(5);
    // input_report_abs(input_data_mid->vinput, ABS_MT_TOUCH_MAJOR, 10);
    // input_report_abs(input_data_mid->vinput, ABS_MT_TOUCH_MINOR, 8);

    // input_report_abs(input_data_mid->vinput, ABS_MT_PRESSURE, 50);
}

void report_end(/*unsigned char finger*/ void)
{
    input_mt_slot(input_data_mid->vinput, 0);
    input_mt_report_slot_state(input_data_mid->vinput, MT_TOOL_FINGER, 0);
    // input_sync(input_data_mid->vinput);
    input_report_key(input_data_mid->vinput, BTN_TOUCH, 0);
    // input_report_key(input_data_mid->vinput, BTN_TOOL_FINGER, 0);

    input_sync(input_data_mid->vinput);
    mdelay(80);
}

void callback(struct timer_list *timer)
{
    static int finger_num = 0;

    input_data_mid->tm.expires = jiffies + 2 * HZ;
    add_timer(&(input_data_mid->tm)); //重新开始计时

    // input_report_key(vinput, FOD_EVENT, fod_status);
    // input_sync(vinput);

    // printk("lfdcn start send1\n");
    // for (finger = 0; finger < finger_index; finger++) {
    //     printk("lfdcn start send2\n");
    //     printk("lfdcn finger=%d\n", finger);
    //     finger_num += 20;
    report_data(finger_num);
    report_end();
    //     report_data(finger, finger_num);
    //     report_end(finger);

    //     // if (finger == (finger_index - 1)) {
    //     // 	report_end(finger);
    //     // } else {
    //     // 	report_data(finger, finger_num);
    //     // }
    // }
    finger_num += 20;

    if (finger_num >= 1059)
        finger_num = 0;
    count_event++;
}

static int vinput_probe(struct platform_device *pdev)
{
    struct input_data *input_data;
    struct input_dev *vinput = NULL;
    int ret = 0;

    input_data =
        devm_kzalloc(&pdev->dev, sizeof(struct input_data), GFP_KERNEL);
    vinput = input_allocate_device();
    input_data->vinput = vinput;
    input_set_drvdata(vinput, input_data);

    vinput->name = "vinput_device";
    vinput->id.bustype = 1;
    vinput->id.vendor = 0x1234;
    vinput->id.product = 0x5678;
    vinput->id.version = 0x86;

    set_bit(EV_SYN, vinput->evbit);
    set_bit(EV_KEY, vinput->evbit);
    set_bit(EV_ABS, vinput->evbit);

    set_bit(BTN_TOUCH, vinput->keybit);
    set_bit(BTN_TOOL_FINGER, vinput->keybit);

    input_set_abs_params(vinput, ABS_MT_POSITION_X, 0, 1079, 0, 0);
    input_set_abs_params(vinput, ABS_MT_POSITION_Y, 0, 2339, 0, 0);

    input_mt_init_slots(vinput, 10, INPUT_MT_DIRECT);

    ret = input_register_device(vinput);

    timer_setup(&(input_data->tm), callback, 0);
    input_data->tm.function = callback;        //指定定时时间到后的回调函数
    input_data->tm.expires = jiffies + 1 * HZ; //定时时间
    add_timer(&(input_data->tm));              //注册定时器

    input_data_mid = input_data;

    return ret;
}

static int vinput_remove(struct platform_device *pdev)
{
    del_timer(&(input_data_mid->tm)); //注销定时器
    input_unregister_device(input_data_mid->vinput);
    input_free_device(input_data_mid->vinput);
    printk(KERN_WARNING "unregister vinput!\n");
    return 0;
}

struct platform_device vinput_device = {
    .name = "virtual click in TP",
    .id = -1,
};

struct platform_driver vinput_driver = {
    .driver = {
        .name = "virtual click in TP",
        .owner = THIS_MODULE,
    },
    .probe = vinput_probe,
    .remove = vinput_remove,
};

static int __init vintput_init(void)
{
    // int ret;

    // set_bit(INPUT_PROP_DIRECT, input_data->vinput->propbit);

    // if (input_data->vinput == NULL) {
    // 	printk(KERN_ERR "input_allocate_device error\n");
    // 	return -ENOMEM;
    // } else {
    // 	printk(KERN_WARNING "input allocate success!\n");
    // }

    // printk(KERN_WARNING "%s : %s : %d - ok.\n", __FILE__, __func__,
    //        __LINE__);

    //定时器

    //初始化input device对象

    //	input_set_abs_params(vinput, ABS_X, 0, 1079, 0, 0);
    //	input_set_abs_params(vinput, ABS_Y, 0, 2340, 0, 0);

    // input_set_abs_params(input_data->vinput, ABS_MT_PRESSURE, 0, 127, 0, 0);
    // input_set_abs_params(input_data->vinput, ABS_MT_TOUCH_MAJOR, 0, 255, 0,
    // 		     0);
    // input_set_abs_params(input_data->vinput, ABS_MT_TOUCH_MINOR, 0, 255, 0,
    //  0);

    // printk("lfdcn vinput->mt->trkid=%d,vinput->mt->num_slots=%d,vinput->mt->slot=%d,vinput->mt->flags=%d",
    //        input_data->vinput->mt->trkid, input_data->vinput->mt->num_slots,
    //        input_data->vinput->mt->slot, input_data->vinput->mt->flags);

    //注册input device对象

    // if (ret != 0)
    // {
    //     printk(KERN_ERR "input_register_device error\n");
    //     goto err_0;
    // }
    // else
    // {
    //     printk(KERN_WARNING "input register success!\n");
    // }

    //     return 0;
    // err_0:
    //     input_free_device(input_data->vinput);
    //     return ret;

    if (platform_device_register(&vinput_device) != 0)
    {
        pr_err("add platform device error\n");
        return -1;
    }

    if (platform_driver_register(&vinput_driver) != 0)
    {
        pr_err("add platform driver error\n");
        return -1;
    }

    return 0;
}

static void __exit vintput_exit(void)
{
    platform_device_unregister(&vinput_device);
    platform_driver_unregister(&vinput_driver);
    // printk(KERN_WARNING "%s : %s : %d - ok.\n", __FILE__, __func__,
    //        __LINE__);
}

module_init(vintput_init);
module_exit(vintput_exit);
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Virtual Device Code");