#include <linux/init.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_gpio.h>

static int __init test_init()
{
    return 0;
}

static void __exit test_exit()
{
}

module_init(test_init);
module_exit(test_exit);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("lfdcn");
MODULE_DESCRIPTION();