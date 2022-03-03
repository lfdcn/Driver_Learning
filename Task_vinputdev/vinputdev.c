#include <linux/module.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>

// #define GPXCON_REG 0x11000c20

struct input_dev *vinputdev;
int irqno;
// void *reg_base;

irqreturn_t input_key_irq_handler(int irqno, void *devid)
{
    //读取数据寄存器
    int value = readl(reg_base + 4) & (1 << 2);

    printk("------%s------\n", __FUNCTION__);

    if(value)
    {
        input_event(vinputdev, EV_KEY, KEY_POWER, 0);
        input_sync(vinputdev);
    }
    else
    {
        input_event(vinputdev, EV_KEY, KEY_POWER, 1);
        input_sync(vinputdev);
    }

    return IRQ_HANDLED;
}

int get_irqno_from_node(void)
{
    //获取到设备树中的节点
    struct device_node *np = of_find_node_by_path("/");
    if (np != 0)
    {
        printk("find node ok\n");
    }
    else
    {
        printk("find node failed\n");
    }

    irqno = irq_of_parse_and_map(np, 0);
    printk("irqno=%d\n", irqno);

    return irqno;
}

static int __init vinputdev_init(void)
{
    int ret;

    //分配一个input device对象
    vinputdev = input_allocate_device(void);

    if (vinputdev == NULL)
    {
        printk(KERN_ERR "input_allocate_device error\n");
        return -ENOMEM;
    }

    //初始化input device对象
    //当前设备能够产生按键数据
    __set_bit(EV_KEY, vinputdev->evbit);
    //当前设备能够产生power按键
    __set_bit(KEY_POWER, vinputdev->keybit);

    //注册input device对象
    ret = input_register_device(vinputdev);
    if (ret != 0)
    {
        printk(KERN_ERR "input_register_device error\n");
        goto err_0;
    }

    //中断
    // irqno = get_irqno_from_node();
    // ret = request_irq(irqno, input_key_irq_handler, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "key3_eint10", NULL);

    // if (ret != 0)
    // {
    //     printk("request_irq error\n");
    //     goto err_1;
    // }
    // reg_base = ioremap(GPXCON_REG, 8);

    return 0;
err_0:
    input_free_device(vinputdev);
    return ret;
// err_1:
//     free_irq(irqno, NULL);
//     return ret;
}

static void __exit vinputdev_exit(void)
{
    // iounmap(reg_base);
    // free_irq(irqno, NULL);
    input_unregister_device(vinputdev);
    input_free_device(vinputdev);
}

module_init(vinputdev_init);
module_exit(vinputdev_exit);
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Virtual Device Code");