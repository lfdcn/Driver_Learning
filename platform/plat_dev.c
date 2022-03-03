#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>

#define GPIO_REG_BASE 0x11400000

#define GPF3_CON GPIO_REG_BASE+0x01E0
#define GPX1_CON GPIO_REG_BASE + 0x0C20
#define GPIO_SIZE 24

//一个设备可能有多个资源
struct resource plat_resource[] = {
    [0] = {
        .start = GPF3_CON,
        .end = GPF3_CON + GPIO_SIZE - 1,
        .flags = IORESOURCE_MEM,
    },
    [1] = {
        .start = GPX1_CON,
        .end = GPX1_CON + GPIO_SIZE - 1,
        .flags = IORESOURCE_MEM,
    },
    //还可以继续添加
};

struct platform_device platform_dev_test = {
    .name = "platform_test1",                       //用于匹配
    .id = -1,
    .num_resources =ARRAY_SIZE(plat_resource),
    .resource = plat_resource,
};

static int __init plat_dev_init(void)
{
    return platform_device_register(&platform_dev_test);
}

static void __exit plat_dev_exit(void)
{
    platform_device_unregister(&platform_dev_test);
}

module_init(plat_dev_init);
module_exit(plat_dev_exit);
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Virtual Platform Device Code");