#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "mpu6050.h"

int main(int argc, char *argv[])
{
    int fd;
    union mpu6050_data data;
    fd = open("/dev/mpu6050_dev", O_RDWR);
    if (fd < 0)
    {
        perror("open");
        exit(1);
    }

    while (1)
    {
        ioctl(fd, IOC_GET_ACCEL, &data);
        printf("accel data: x = %d, y = %d, z = %d\n", data.accel.x, data.accel.y, data.accel.z);

        ioctl(fd, IOC_GET_GYRO, &data);
        printf("gyro data: x = %d, y = %d, z = %d\n", data.gyro.x, data.gyro.y, data.gyro.z);
        sleep(1);
    }

    return 0;
}
