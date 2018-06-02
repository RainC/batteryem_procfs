#include <stdio.h>
#include <signal.h>

static void sig_usr(int signo) ; 

int main(void){
	// 내 pid를 pid_th 에다가 작성
	// ofstream file ("./battery/pid_th");
	// file << (long)getpid();
	// file.close()

	// Threshold value 는 20
	// 20 이하일 때 절전모드
	// 아닐 때 표준모드
	printf("getpid : %d\n", getpid());
	
	FILE *fp;
	FILE *fp2;
	fp = fopen("/proc/pidnum", "w");
	fprintf(fp, "%d", getpid());
	fclose(fp);


	fp2 = fopen("/proc/threshold", "w");
	fprintf(fp2, "%d", 20);
	fclose(fp2);

	// 신호처리 예외
	if (signal(SIGUSR1, sig_usr) == SIG_ERR)
		printf("can't catch SIGUSR1"); 

	if (signal(SIGUSR2, sig_usr) == SIG_ERR)
		printf("can't catch SIGUSLR2");

	for( ;; ) pause();
}

static void sig_usr(int signo) {
	if (signo == SIGUSR1)
		// 절전모드 감지
		printf("received SIGUSR1 절전\n");
	else if (signo == SIGUSR2)
		// 표준모드 감지
		printf("received SIGUSR2 표준\n");
	else printf("received signal %d\n", signo);
	return;
}

