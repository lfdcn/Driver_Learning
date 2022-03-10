#include <linux/device/driver.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
//#include <linux/videodev2.h>

struct mpu6050_struct
{
    int dev_major;
    struct device *dev;
    struct class *cls;
    struct i2c_client *client;
};

struct mpu6050_struct *mpu6050_dev;

const struct file_operations mpu6050_fops = {

};

int mpu6050_write_bytes(const struct i2c_client *client, const char *buf, int count)
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

int mpu6050_read_bytes(const struct i2c_client *client, const char *buf, int count)
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

int mpu6050_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    mpu6050_dev = kzalloc(sizeof(struct mpu6050_struct), GFP_KERNEL);

    mpu6050_dev->client = client;
    mpu6050_dev->dev_major = register_chrdev(0, "mpu6050_test", &mpu6050_fops);
    mpu6050_dev->cls = class_create(THIS_MODULE, "mpu6050_cls");
    mpu6050_dev->dev = device_create(mpu6050_dev->cls, NULL, MKDEV(mpu6050_dev->dev_major, 0), NULL, "mpu6050_dev");
}

int mpu6050_remove(struct i2c_client *client)
{
    device_destroy(mpu6050_dev->cls, MKDEV(mpu6050_dev->dev_major, 0));
    class_destroy(mpu6050_dev->cls);
    unregister_chrdev(mpu6050_dev->dev_major, "mpu6050_test");
    kfree(mpu6050_dev);
}

//依赖于设备树
const struct of_device_id *of_mpu6050_id[] = {
    {
        .compatible = "test,mpu6050",
    } {/*nothing to be done*/},
};

//不用设备树
struct i2c_device_id *mpu6050_id_table[] = {
    {"i2c_demo", 0x1111},
    {/*nothing to be done*/},
};

struct i2c_driver mpu6050 = {
    .probe = mpu6050_probe,
    .remove = mpu6050_remove,
    .driver = {
        .name = "i2c_driver_test",
        .of_match_table = of_match_ptr(of_mpu6050_id),
        .module = THIS_MODULE,
    },
    .id_table = mpu6050_id_table, //非设备树匹配
};

static int __init viic_init(void)
{
    return i2c_add_driver(&mpu6050);
}

static void __exit viic_exit(void)
{
    i2c_del_driver(&mpu6050);
}

module_init(viic_init);
module_exit(viic_exit);
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Virtual IIC Device Code");
