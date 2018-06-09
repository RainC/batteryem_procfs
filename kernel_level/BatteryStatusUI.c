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
	int set_threshold = atoi (argv[1]); // threshold
	int set_test_value = atoi(argv[2]); // test_value
	// read 먼저 
	// 그리고 write 해보기
	// test_level write, threshold write implements
	// ioctl = test_level 값 설정
	// chr_write = threshold 값 설정
	// 사용 방법
	// ./BatteryStatusUI  <set_test_value> <set_threshold>
	
	device = open(DEVICE_FILE_NAME, O_RDWR | O_NDELAY) ;
	if (device >= 0 ) {
        printf("Device file open\n");
		printf("set_test_value %d", set_test_value);
		// ioctl (device, 0);	
		sprintf(wbuf, "%d", set_threshold); // assigned int value to chr[XXX]
		write(device,wbuf , 0); // set_threshold 설정
		ioctl(device,0);
		sprintf(wbuf, "%d", set_test_value); // assigned int value to chr[XXX]
		write(device,wbuf , 1); // set_test_value 설정
		printf("set_threshold_value : %d\n", set_threshold); 
	
		read(device, rbuf, 0) ;
		// printf("read value %s \n", rbuf);
		// Read is not enabled (동작 안함)
		ioctl(device,0);
    } else {
        perror("Error open device (sudo needed)");
    }
    return 0 ;
}