
## 목차
- 제출자 정보
- 시행 착오
- 개발 환경
- 설계 내용
- 구현 과정의 결정 사항
- 시연 방법
- 성공 결과
 
## 시행 착오
- 짧은 후기
- `Kernel level 부터 시작해서 딥해서 너무너무어려웠습니다.....`
- 시행착오 #1 dmesg

![](https://i.imgur.com/V1pWam9.png)
- 자세한 원인은 모르겠지만, 다시 모듈 컴파일 해서 insmod 명령어로 모듈 설치 시도를 했지만 `stuck` 상태에 빠졌습니다. `stuck` 상태에 빠질 때 저 오류 메세지가 나왔습니다.

- 오류 메세지 핵심
```
[10033.899625] Unable to handle kernel paging request at 
virtual address 7f455364
[10033.899648] pgd = ad4b0000
[10033.899657] [7f455364] *pgd=3596b811, *pte=00000000, *ppte=00000000
```

- 원인
```
[  623.958639] ------------[ cut here ]------------
[  623.958683] WARNING: CPU: 1 PID: 1108 at fs/proc/generic.c:345 proc_register+0x10c/0x138
[  623.958693] proc_dir_entry '/proc/battery_test' already registered
[  623.958701] Modules linked in: procfs(O+) fuse rfcomm cmac bnep hci_uart btbcm bluetooth brcmfmac brcmutil cfg80211 rfkill snd_bcm2835 snd_pcm snd_timer snd bcm2835_gpiomem w1_gpio wire cn uio_pdrv_genirq uio fixed i2c_dev ip_tables x_tables ipv6 [last unloaded: procfs]
[  623.958885] CPU: 1 PID: 1108 Comm: insmod Tainted: G        W  O    4.9.59-v7+ #1047
[  623.958890] Hardware name: BCM2835
[  623.958918] [<8010fb3c>] (unwind_backtrace) from [<8010c058>] (show_stack+0x20/0x24)
[  623.958951] [<8010c058>] (show_stack) from [<80456764>] (dump_stack+0xd4/0x118)
[  623.958974] [<80456764>] (dump_stack) from [<8011d348>] (__warn+0xf8/0x110)
[  623.958989] [<8011d348>] (__warn) from [<8011d3a8>] (warn_slowpath_fmt+0x48/0x50)
[  623.959020] [<8011d3a8>] (warn_slowpath_fmt) from [<802e0454>] (proc_register+0x10c/0x138)
[  623.959044] [<802e0454>] (proc_register) from [<802e06cc>] (proc_create_data+0x88/0xcc)
[  623.959070] [<802e06cc>] (proc_create_data) from [<7f459248>] (init_module+0x44/0xa8 [procfs])
[  623.959127] [<7f459248>] (init_module [procfs]) from [<80101bf0>] (do_one_initcall+0x50/0x184)
[  623.959147] [<80101bf0>] (do_one_initcall) from [<8020a294>] (do_init_module+0x74/0x1e4)
[  623.959166] [<8020a294>] (do_init_module) from [<801a37bc>] (load_module+0x19cc/0x2108)
[  623.959185] [<801a37bc>] (load_module) from [<801a40d4>] (SyS_finit_module+0x9c/0xac)
[  623.959202] [<801a40d4>] (SyS_finit_module) from [<801080c0>] (ret_fast_syscall+0x0/0x1c)
[  623.959209] ---[ end trace 8385fdb6a7bf4237 ]---
```

- 모듈 삭제 시도
```
pi@raspberrypi:~/rubyrain/batteryem_procfs $ rmmod procfs
rmmod: ERROR: Module procfs is in use
pi@raspberrypi:~/rubyrain/batteryem_procfs $ rmmod procfs -f
rmmod: ERROR: ../libkmod/libkmod-module.c:793 kmod_module_remove_module() could not remove 'procfs': Operation not permitted
rmmod: ERROR: could not remove module procfs: Operation not permitted
pi@raspberrypi:~/rubyrain/batteryem_procfs $ sudo rmmod procfs -f
rmmod: ERROR: ../libkmod/libkmod-module.c:793 kmod_module_remove_module() could not remove 'procfs': Device or resource busy
rmmod: ERROR: could not remove module procfs: Device or resource busy
pi@raspberrypi:~/rubyrain/batteryem_procfs $ rmmod procfs
rmmod: ERROR: Module procfs is in use
pi@raspberrypi:~/rubyrain/batteryem_procfs $ ^C
pi@raspberrypi:~/rubyrain/batteryem_procfs $ rmmod -f procfs
rmmod: ERROR: ../libkmod/libkmod-module.c:793 kmod_module_remove_module() could not remove 'procfs': Operation not permitted
rmmod: ERROR: could not remove module procfs: Operation not permitted
pi@raspberrypi:~/rubyrain/batteryem_procfs $ sudo rmmod -f procfs
rmmod: ERROR: ../libkmod/libkmod-module.c:793 kmod_module_remove_module() could not remove 'procfs': Device or resource busy
rmmod: ERROR: could not remove module procfs: Device or resource busy
pi@raspberrypi:~/rubyrain/batteryem_procfs $ ^C
```

- 모듈 삭제 시도 후 설치 해도 오류가 나는데, 이럴땐 재부팅말곤 해결 방법이 없어 보입니다. 오류날 때 마다 재부팅으로 해결 했습니다.
```
sudo shutdown -r now
```
- 시행착오 #2 sudo 를 붙여주니까 됨 왜냐 `/dev/chr_dev` 영역은 `pi` 유저로는 접근 안되기 때문에 sudo 가 필요함
```
pi@raspberrypi:~/rubyrain/batteryem_procfs/new/device $ ./battery
1pi@raspberrypi:~/rubyrain/batteryem_procfs/new/device $ sudo ./battery 1
Device file Open
Write Value is ::::: Write buffer data
Read value is ::::: Read buffer data
pi@raspberrypi:~/rubyrain/batteryem_procfs/new/device $
```

- 시행착오 #3 `procfs + battery device driver` 합침 시도 오류
```
Message from syslogd@raspberrypi at Jun  2 19:21:42 ...
 kernel:[ 1592.961827] bf00: a78abf3c a78abf10 80290714 8028fea0 00020000 00020000 b8d9e000 00000000

Message from syslogd@raspberrypi at Jun  2 19:21:42 ...
 kernel:[ 1592.961842] bf20: 00000005 00000002 ffffff9c 00000000 00000003 ffffff9c b8d9e000 00000005

Message from syslogd@raspberrypi at Jun  2 19:21:42 ...
 kernel:[ 1592.961858] bf40: a78abf94 a78abf50 8026f3e0 80281420 00000001 000003e8 00000005 00020000

Message from syslogd@raspberrypi at Jun  2 19:21:42 ...
 kernel:[ 1592.961873] bf60: 00000000 00000004 00000100 00000001 00000000 00000000 00015360 00000005
...
```

### 개발 환경
- RPi 3 
```
pi@raspberrypi:~/rubyrain/batteryem_procfs/new $ cat /proc/version
Linux version 4.9.59-v7+ (dc4@dc4-XPS13-9333)
(gcc version 4.9.3 (crosstool-NG crosstool-ng-1.22.0-88-g8460611) ) #
1047 SMP Sun Oct 29 12:19:23 GMT 2017
pi@raspberrypi:~/rubyrain/batteryem_procfs/new $
```


### 설계 내용
- 우선 과제 요구사항에 따라 최대한 설계한 내용을 반영하려고 했습니다. 
- 제가 설계한 방향은 아래와 같습니다.
- `device_module` 폴더 내용은 https://sysplay.in/blog/tag/ioctl/ 를 참고했습니다. ioctl 의 읽기/쓰기 부분을 조금 더 깊게 보기 위해서 참고하였습니다. 여기서 static 한 변수들을 ioctl 로 I/O 를 할 수 있게 로직을 작성하려고 했습니다. 예제 코드는 작동은 되는 듯 하나, 실제로 procfs module 에 합치면서 문제가 발생했습니다. 시간이 너무 오래 걸려서, 이 부분까지는 진행하지 못하였습니다. 
---
- device_module/
    - Makefile
    - device.c
    - query_ioctl.h
- procfs_module/
    - Makefile
    - procfs.c
- Emulator
- powermanager.c

#### PowerManager 처리

![](https://i.imgur.com/47o0tBO.png)

#### procfs 모듈에서 SIGUSR1, SIGUSR2 신호 전송

![](https://i.imgur.com/7z6l0tp.png)



#### procfs 모듈에서 file operations read/write 연동처리
```
static const struct file_operations my_proc_fops = {
        .write = test_level_write,
        .read = test_level_read,
};

static const struct file_operations pidnum_ops = {
        .write = pidnum_write,
        .read = pidnum_read,
};

static const struct file_operations threshold_ops = {
        .write = threshold_write,
        .read = threshold_read,
};
```

#### Device Driver read/write 연동 처리 - write 만 구현
```
struct file_operations chr_fops = {
        owner: THIS_MODULE,
        unlocked_ioctl: chr_ioctl,
        write: chr_write,
        read: chr_read,
        open: chr_open,
        release: chr_release
};
```

#### BatteryStatusUI 에서 Device driver 의 `threshold` 값 설정하는 방법
```
int set_threshold = 10; 
printf("Device file open\n");
printf("set_threshold value %d\n", set_threshold);
sprintf(wbuf, "%d", set_threshold); // assigned int value to chr[XXX]
write(device,wbuf , 0); // set_threshold 설정
```

#### BatteryStatusUI 에서 Device driver 의 `test_value` 값 설정하는 방법
```
printf("set_test_value : %d\n", set_test_value);
ioctl (device, 1);	// Set receivemode to test_value
sprintf(wbuf, "%d", set_test_value); // assigned int value to chr[XXX]
write(device,wbuf , 1); // set_test_value 설정
read(device, rbuf, 0) ;
ioctl(device,0); // 다음 StatusUI 실행을 위해 ioctl 0 으로 값 설정(중요)
```
- Device Driver 에서 test_value 값이 세팅되면 ioctl_mode 를 0으로 다시 변경



## 구현 과정의 결정 사항
![](https://i.imgur.com/FhF6Sbl.png)


- `구현 성공 Details`
    - `Register Setting` - `PowerManager` 가 procfs 모듈을 통해 pidnum, threshold 값을 초기화 해줍니다. pidnum 은 `PowerManager` 의 Process ID, threshold 는 `20` 으로 설정하였습니다.
    - `Kernel Module` - procfs 모듈이 아래와 같이 변수관리, 파일 접근을 할 수 있게 구현하였습니다.
    - test_level 은 `/proc/battery_test` 
    - threshold 은 `/proc/threshold`
    - pidnum 은 `/proc/pidnum`
    - `Emulator` 은 미리 준비된 스크립트에서 `/proc/` 로 Redirection 되게 수정하였습니다. (ecampus)
    - `NotifierLogic` 은 procfs 모듈 안에 있으며, `battery_test`, `threshold` 값을 모니터링 하고,`/proc/pidnum` 값을 통해 `PowerManager` 에 `SIGUSR1`, `SIGUSR2` 신호를 보냅니다.
    - `BatteryStatusUI` 에서 `DeviceDriver` 를 통해 `ioctl` Command 명령이 `0` 이면 threshold,  `1` 이면 test_value 값을 `chr_write` 함수에서 설정해 줍니다.
- `구현 실패 Details`
    - Battery Status UI 에서 Read를 구현하지 못했습니다. `test_value` , `threshold` Read 

### 시연 방법
- 라즈베리파이 환경이여야 합니다. `https://github.com/notro/rpi-source/wiki` 를 참고해 주세요. 
- 아래와 같이 진행하면, 시연이 가능합니다.
- 먼저, 소스를 다운로드 합니다. 그리고 procfs 모듈을 먼저 설치해 줍니다.
```
cd sourceFolder/kernel_level/procfs_module/
```
-  procfs 모듈을 make 해줍니다.
```
make
```
- procfs 모듈을 설치해 줍니다.
```
sudo insmod procfs.ko
```
- device 모듈 연동을 위한 디바이스 파일 생성
```
sudo mknod /dev/chr_dev c 240 0 
```
- PowerManager 를 컴파일 해줍니다.
```
cd sourceFolder/kernel_level/
```
```
gcc -o Powermanager Powermanager.c
```
- PowerManager 를 실행해 줍니다.
```
./Powermanager
```
- 다른 창에서 Emulator 를 실행합니다.
```
sudo ./Emulator 
```
- 이렇게 진행하면, Emulator 는 /proc/test_level 에 파일을 작성하면 , 커널에서 level 을 감지하고, Powermanager 에서 절전/표준 신호를 받게 됩니다.

- BatteryStatusUI 실행 방법
```
gcc -o BatteryStatusUI BatteryStatusUI.c
```

```
sudo ./BatteryStatusUI <Threshold> <test_value>
```


```
sudo ./BatteryStatusUI 10 50
Device file open
set_threshold value 10
set_test_value : 50
```




### 성공 결과 (BatteryEmulator)

![](https://i.imgur.com/SOERgej.png)

- 왼쪽 `Powermanager`
    - 절전모드,표준모드 같이 보이는 모습 있음
- 오른쪽 `Emulator`
- 왼쪽 아래 `dmesg` 커널 Message


### 성공 결과 (Device Driver)
![](https://i.imgur.com/7bU0orv.png)

- 왼쪽 `BatteryStatusUI`
- BatteryStatus UI 인자값 옵션
    - `BatteryStatusUI <Threshold> <test_value>`
- 오른쪽 `PowerManager`
- 중앙쪽엔 `dmesg` , 드레그 된 부분 Check
