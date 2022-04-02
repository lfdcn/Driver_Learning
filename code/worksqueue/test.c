#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>

struct wq_struct
{
    struct work_struct work;
    struct delayed_work dl_work;
};

static struct wq_struct wq_op;

void wq_printf(struct work_struct *work)
{
    printk("=====%s=====", __func__);
}

static int __init test_init(void)
{
    INIT_WORK(&wq_op.work, wq_printf);

    if (0 == schedule_work(&wq_op.work))
        printk("failed");

    return 0;
}

static void __exit test_exit(void)
{
    cancel_work_sync(&wq_op.work);
}

module_init(test_init);
module_exit(test_exit);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("lfdcn");
MODULE_DESCRIPTION("worksqueue_test");
