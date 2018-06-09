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
	// ./BatteryStatus <set_threshold> <set_test_value>
	sprintf(wbuf, "%d", set_threshold); // assigned int value to chr[XXX]
	device = open(DEVICE_FILE_NAME, O_RDWR | O_NDELAY) ;
	if (device >= 0 ) {
        printf("Device file open\n");
		printf("Setting test_value, value is %d", set_test_value);
		
		ioctl (device, set_test_value);
		printf("Setting threshold value, value is %d", set_threshold); 
		write(device,wbuf , 10); // set_test_value 설정
		printf("write requested %s \n", argv[2]);
		read(device, rbuf, 10) ;
		printf("read value %c \n", rbuf);
    } else {
        perror("Error open device (sudo needed)");
    }
    return 0 ;
}