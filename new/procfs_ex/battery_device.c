#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unisted.h>

#define DEVICE_FILE_NAME "/dev/chr_dev"

int main(int argc, char *argv[]) {
    int device;
    char wbuf[128] = "Write buffer data";
    char rbuf[128] = "Read buffer data";
    int n = atoi(argv[1]);

    device = open(DEVICE_FILE_NAME, 0_RDWR | O_NDELAY);
    if (device >=0 ) {
        printk(KERN_ALERT "Device file Open\n");
        ioctl(device ,n);
        write(device, wbuf, 10); 
        printk(KERN_ALERT "Write Value is %s\n", wbuf);
        read(device, rbuf,10);
        printk(KERN_ALERT "Read value is %s\n", rbuf);

    } else {
        printk(KERN_ALERT "Device open fail \n");
    }
    return 0;
}