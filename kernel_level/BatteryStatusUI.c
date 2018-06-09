#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>


#define DEVICE_FILE_NAME "/dev/chr_dev"

 

int main(int argc, char *argv[]) {
    int device;
   
	char wbuf[128] = "" ; 
	char rbuf[128] = "";
	int n = atoi (argv[1]);
	int n2 = atoi(argv[2]);
	int set_threshold = 10; // default_threshold
	// read 먼저 
	// 그리고 write 해보기
	// test_level write, threshold write implements
	device = open(DEVICE_FILE_NAME, O_RDWR | O_NDELAY) ;
	if (device >= 0 ) {
        printf("Device file open\n");
		printf("Setting threshold value, value is %d", set_threshold);
        ioctl(device , n );
		ioctl (device, n2); // argv 2  to set set_test_value
		write(device,argv[2] , set_threshold);
		printf("write requested %s \n", argv[2]);
		read(device, rbuf, 10) ;
		printf("read value %c \n", rbuf);
    } else {
        perror("Error open device (sudo needed)");
    }
    return 0 ;
}