#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>

int plat_driver_probe(struct platform_device *pdev)
{

    return 0;
}
int plat_driver_remove(struct platform_device *pdev)
{

    return 0;
}

const struct platform_device_id id_table_test[]={
    {"platform_test1",0x1314},
    {"platform_test2",0x9413},
};

struct platform_driver platform_drv_test = {
    .probe = plat_driver_probe,
    .remove = plat_driver_remove,
    .driver={
        .name="fucking_plat_drv",
    },
    .id_table = id_table_test,
};

static int __init plat_drv_init(void)
{
    return platform_driver_register(&platform_drv_test);
}

static void __exit plat_drv_exit(void)
{
    platform_driver_unregister(&platform_drv_test);
}

module_init(plat_drv_init);
module_exit(plat_drv_exit);
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Virtual IIC Device Code");