#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/fnctl.h>
#include <sys/types.h>
#include <unistd.h>


#define DEVICE_FILE_NAME "/dev/chr_dev"

typedef struct
{
	int test_battery_value, threshold;
} query_arg_t;


// ioctl query implementation
void get_vars(int fd)
{
	query_arg_t q;

	if (ioctl(fd, 1, &q) == -1)
	{
		perror("query_apps ioctl get - error");
	}
	else
	{
		printf("test_battery_value : %d\n", q.test_battery_value);
		printf("threshold: %d\n", q.threshold);
	}
}

int main() {
    int device;
    device = open(DEVICE_FILE_NAME, O_RDWR | O_NDELAY) ;
    if (device >= 0 ) {
        printf("Device file open");
        get_vars(device);
    } else {
        perror("Error open device (sudo needed)");
    }
    return 0 ;
}