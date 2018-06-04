#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>


#define DEVICE_FILE_NAME "/dev/chr_dev"

 

int main() {
    int device;
    device = open(DEVICE_FILE_NAME, O_RDWR | O_NDELAY) ;
	char wbuf[128] = "to kernel " ; 
	char rbuf[128] = "efefef";
	int n = atoi (argv[1]);
	
	if (device >= 0 ) {
        printf("Device file open");
        ioctl(device , n );
		write(device,wbuf , 10);
		printf("write requested", wbuf);
		read(device ,rbuf, 10);
		printf("read value %s ", rbuf);
    } else {
        perror("Error open device (sudo needed)");
    }
    return 0 ;
}