#include <linux/device/driver.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
//#include <linux/videodev2.h>

struct vi2c_struct
{
    int dev_major;
    struct device *dev;
    struct class *cls;
    struct i2c_client *client;
};

struct vi2c_struct *vi2c_dev;

const struct file_operations vi2c_fops = {

};

int vi2c_write_bytes(const struct i2c_client *client, const char *buf, int count)
{
    int ret;

    struct i2c_adapter *adapter = client->adapter;
    struct i2c_msg msg;

    msg.addr = client->addr;
    msg.flags = 0x00;
    msg.len = count;
    msg.buf = buf;

    ret = i2c_transfer(adapter, msg, 1);
    return ret == 1 ? count : ret;
}

int vi2c_read_bytes(const struct i2c_client *client, const char *buf, int count)
{
    int ret;

    struct i2c_adapter *adapter = client->adapter;
    struct i2c_msg msg;

    msg.addr = client->addr;
    msg.flags |= I2C_M_RD;
    msg.len = count;
    msg.buf = buf;

    ret = i2c_transfer(adapter, msg, 1);
    return ret == 1 ? count : ret;
}

int vi2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    vi2c_dev = kzalloc(sizeof(struct vi2c_struct), GFP_KERNEL);

    vi2c_dev->client = client;
    vi2c_dev->dev_major = register_chrdev(0, "vi2c_test", &vi2c_fops);
    vi2c_dev->cls = class_create(THIS_MODULE, "vi2c_cls");
    vi2c_dev->dev = device_create(vi2c_dev->cls, NULL, MKDEV(vi2c_dev->dev_major, 0), NULL, "vi2c_dev");
}
int vi2c_remove(struct i2c_client *client)
{
    device_destroy(vi2c_dev->cls, MKDEV(vi2c_dev->dev_major, 0));
    class_destroy(vi2c_dev->cls);
    unregister_chrdev(vi2c_dev->dev_major, "vi2c_test");
    kfree(vi2c_dev);
}

//依赖于设备树
const struct of_device_id *of_vi2c_id[] = {
    {
        .compatible = "test,vi2c",
    } {/*nothing to be done*/},
};

//不用设备树
struct i2c_device_id *vi2c_id_table[] = {
    {"i2c_demo", 0x1111},
    {/*nothing to be done*/},
};

struct i2c_driver vi2c = {
    .probe = vi2c_probe,
    .remove = vi2c_remove,
    .driver = {
        .name = "i2c_driver_test",
        .of_match_table = of_match_ptr(of_vi2c_id),
        .module = THIS_MODULE,
    },
    .id_table = vi2c_id_table, //非设备树匹配
};

static int __init viic_init(void)
{
    return i2c_add_driver(&vi2c);
}

static void __exit viic_exit(void)
{
    i2c_del_driver(&vi2c);
}

module_init(viic_init);
module_exit(viic_exit);
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Virtual IIC Device Code");
