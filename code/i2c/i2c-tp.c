#include <linux/device.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

struct tp_device
{
    int dev_major;
    struct device *dev;
    struct class *cls;
    struct i2c_client *client;
};
struct tp_device *tp_dev;

int tp_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    tp_dev = kzalloc(sizeof(struct tp_device), GFP_KERNEL);

    tp_dev->client = client;
    tp_dev->dev_major =
        register_chrdev(0, "tp_dev", &mpu6050_fops);
    tp_dev->cls = class_create(THIS_MODULE, "tp_cls");
    tp_dev->dev = device_create(tp_dev->cls, NULL,
                                MKDEV(tp_dev->dev_major, 0), NULL,
                                "tp_dev");
    printk("=====%s=====\n", __func__);
    return 0;
}

int tp_remove(struct i2c_client *client)
{
    device_destroy(tp_dev->cls, MKDEV(tp_dev->dev_major, 0));
    class_destroy(tp_dev->cls);
    unregister_chrdev(tp_dev->dev_major, "tp_dev");
    kfree(tp_dev);
    printk("=====%s=====\n", __func__);

    return 0;
}

//依赖于设备树
const struct of_device_id of_tp_id[] = {
    {.compatible = "allwinner,tp"},
    {},
};

//不用设备树
struct i2c_device_id tp_id_table[] = {
    {"i2c_demo", 0x1111},
};

struct i2c_driver tp = {
    .probe = tp_probe,
    .remove = tp_remove,
    .driver = {
        .name = "i2c_driver_test",
        .of_match_table = of_match_ptr(of_tp_id),
    },
    .id_table = tp_id_table, //非设备树匹配
};

static int __init tp_init(void)
{
    return i2c_add_driver(&tp_dev);
}

static void __exit tp_exit(void)
{
    i2c_del_driver(&tp_dev);
}

module_init(tp_init);
module_exit(tp_exit);
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Graduate Designed for TP by HZX");
