#include <linux/module.h>
#include <linux/init.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>

irqreturn_t key_irq_handler(int irqno, void *devid)
{
    printk("------%s------\n", __FUNCTION__);

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

    int irqno = irq_of_parse_and_map(np, 0);
    printk("irqno=%d\n", irqno);

    return irqno;
}

// cat /proc/interrupts查看中断
static int __init key_drv_init(void)
{
    int ret;
    //如何获得中断号
    int irqno;

    irqno = get_irqno_from_node();
    ret = request_irq(irqno, key_irq_handler, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "key3_eint10", NULL);

    if (ret != 0)
    {
        printk("request_irq error\n");
        return -EBUSY;
    }

    return 0;
}

static void __exit key_drv_exit(void)
{
    //最后一个参数需要与request_irq一致，在这里就是NULL
    free_irq(irqno,NULL);
}

module_init(key_drv_init);
module_exit(key_drv_exit);
MODULE_LICENCE("GPL");
MODULE_DESCRIPT("key driver learn");
