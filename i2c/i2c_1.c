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

#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H 0x41
#define TEMP_OUT_L 0x42
#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48

union mpu6050_data
{
	struct
	{
		short x;
		short y;
		short z;
	} accel;
	struct
	{
		short x;
		short y;
		short z;
	} gyro;
	short temp;
};

#define IOC_GET_ACCEL _IOR('M', 0x34, union mpu6050_data)
#define IOC_GET_GYRO _IOR('M', 0x35, union mpu6050_data)
#define IOC_GET_TEMP _IOR('M', 0x36, union mpu6050_data)

struct mpu6050_struct
{
	int dev_major;
	struct device *dev;
	struct class *cls;
	struct i2c_client *client;
};

struct mpu6050_struct *mpu6050_dev;

int mpu6050_read_reg_byte(struct i2c_client *client, char reg)
{
	int ret;

	struct i2c_adapter *adapter = client->adapter;
	struct i2c_msg msg[2];

	char rxbuf[1];

	//写
	msg[0].addr = client->addr;
	msg[0].flags = 0;
	msg[0].len = 1;
	msg[0].buf = &reg;

	//读
	msg[1].addr = client->addr;
	msg[1].flags = I2C_M_RD;
	msg[1].len = 1;
	msg[1].buf = rxbuf;

	ret = i2c_transfer(adapter, msg, 1);
	if (ret < 0)
	{
		printk("i2c_transfer read error\n");
		return ret;
	}

	return rxbuf[0];
}

int mpu6050_open(struct inode *inode, struct file *filp)
{
	return 0;
}
int mpu6050_release(struct inode *inode, struct file *filp)
{
	return 0;
}
long mpu6050_unlock_ioctl(struct file *filp, unsigned int cmd, unsigned long args)
{
	union mpu6050_data data;

	switch (cmd)
	{
	case IOC_GET_ACCEL:
		data.accel.x = mpu6050_read_reg_byte(mpu6050_dev->client, ACCEL_XOUT_L);
		data.accel.x |= mpu6050_read_reg_byte(mpu6050_dev->client, ACCEL_XOUT_H) << 8;

		data.accel.y = mpu6050_read_reg_byte(mpu6050_dev->client, ACCEL_YOUT_L);
		data.accel.y |= mpu6050_read_reg_byte(mpu6050_dev->client, ACCEL_YOUT_H) << 8;

		data.accel.z = mpu6050_read_reg_byte(mpu6050_dev->client, ACCEL_ZOUT_L);
		data.accel.z |= mpu6050_read_reg_byte(mpu6050_dev->client, ACCEL_ZOUT_H) << 8;
		break;
	case IOC_GET_GYRO:
		data.gyro.x = mpu6050_read_reg_byte(mpu6050_dev->client, GYRO_XOUT_L);
		data.gyro.x |= mpu6050_read_reg_byte(mpu6050_dev->client, GYRO_XOUT_H) << 8;

		data.gyro.y = mpu6050_read_reg_byte(mpu6050_dev->client, GYRO_YOUT_L);
		data.gyro.y |= mpu6050_read_reg_byte(mpu6050_dev->client, GYRO_YOUT_H) << 8;

		data.gyro.z = mpu6050_read_reg_byte(mpu6050_dev->client, GYRO_ZOUT_L);
		data.gyro.z |= mpu6050_read_reg_byte(mpu6050_dev->client, GYRO_ZOUT_H) << 8;
		break;
	case IOC_GET_TEMP:
		data.temp = mpu6050_read_reg_byte(mpu6050_dev->client, TEMP_OUT_L);
		data.temp |= mpu6050_read_reg_byte(mpu6050_dev->client, TEMP_OUT_H) << 8;
		break;
	default:
		printk("invaild cmd\n");
		return -EINVAL;
	}
	if (copy_to_user((void __user *)args, &data, sizeof(data)) > 0)
	{
		return -EFAULT;
	}
	return 0;
}

const struct file_operations mpu6050_fops = {
	.open = mpu6050_open,
	.release = mpu6050_release,
	.unlocked_ioctl = mpu6050_unlock_ioctl,
};

int mpu6050_write_bytes(const struct i2c_client *client, const char *buf,
						int count)
{
	int ret;

	struct i2c_adapter *adapter = client->adapter;
	struct i2c_msg msg;

	msg.addr = client->addr;
	msg.flags = 0x00;
	msg.len = count;
	msg.buf = buf;

	ret = i2c_transfer(adapter, &msg, 1);
	return ret == 1 ? count : ret;
}

int mpu6050_read_bytes(const struct i2c_client *client, const char *buf,
					   int count)
{
	int ret;

	struct i2c_adapter *adapter = client->adapter;
	struct i2c_msg msg;

	msg.addr = client->addr;
	msg.flags = I2C_M_RD;
	msg.len = count;
	msg.buf = buf;

	ret = i2c_transfer(adapter, &msg, 1);
	return ret == 1 ? count : ret;
}

int mpu6050_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	mpu6050_dev = kzalloc(sizeof(struct mpu6050_struct), GFP_KERNEL);

	mpu6050_dev->client = client;
	mpu6050_dev->dev_major =
		register_chrdev(0, "mpu6050_test", &mpu6050_fops);
	mpu6050_dev->cls = class_create(THIS_MODULE, "mpu6050_cls");
	mpu6050_dev->dev = device_create(mpu6050_dev->cls, NULL,
									 MKDEV(mpu6050_dev->dev_major, 0), NULL,
									 "mpu6050_dev");
	printk("=====%s=====", __func__);
	return 0;
}

int mpu6050_remove(struct i2c_client *client)
{
	device_destroy(mpu6050_dev->cls, MKDEV(mpu6050_dev->dev_major, 0));
	class_destroy(mpu6050_dev->cls);
	unregister_chrdev(mpu6050_dev->dev_major, "mpu6050_test");
	kfree(mpu6050_dev);
	printk("=====%s=====", __func__);

	return 0;
}

//依赖于设备树
const struct of_device_id of_mpu6050_id[] = {
	{.compatible = "allwinner,mpu6050"},
	{},
};

//不用设备树
struct i2c_device_id mpu6050_id_table[] = {
	{"i2c_demo", 0x1111},
};

struct i2c_driver mpu6050 = {
	.probe = mpu6050_probe,
	.remove = mpu6050_remove,
	.driver = {
		.name = "i2c_driver_test",
		.of_match_table = of_match_ptr(of_mpu6050_id),
	},
	.id_table = mpu6050_id_table, //非设备树匹配
};

static int __init mpu6050_init(void)
{
	return i2c_add_driver(&mpu6050);
}

static void __exit mpu6050_exit(void)
{
	i2c_del_driver(&mpu6050);
}

module_init(mpu6050_init);
module_exit(mpu6050_exit);
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Virtual IIC Device Code");
