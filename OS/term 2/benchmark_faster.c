#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <sys/mman.h>
#include <unistd.h>

#define  __USE_GNU
#include <sched.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#define COL 500
#define ROW 500

#define __NR_sched_transfer 316

int main(void){

	struct timespec start, end;
	
	int h,i,j,k;
	int matrixA[ROW][COL];
	int matrixB[ROW][COL];
	int matrixC[ROW][COL];
	
	int created_thread = 0;
	int pid = getpid();
	int ret;
	
		// NOOTE : All threads execute code from here down!
		cpu_set_t mask;
		
		// CPU_ZERO initializes all the bits in the mask to zero
		CPU_ZERO(&mask);

		// CPU_SET sets only the bits in the mask to zero
		CPU_SET(created_thread, &mask);

		// sched_setaffinity returns 0 in success
		if(sched_setaffinity(0, sizeof(mask), &mask) == -1){

			printf("WARNING: Could not set CPU Affinity, Continuing...\n");
		}

		printf("This Process's PID :: %d\n", pid);
		ret = syscall(__NR_sched_transfer, pid);
		printf("Benchmark_faster is called...!\n");

		clock_gettime(CLOCK_MONOTONIC, &start);

		for(h = 0; h < 50; h++){
			for(i = 0; i < ROW; i++){
				for(j = 0; j < COL; j++){
					for(k = 0; k < COL; k++){

						matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
					}
				}
			}
		}

		clock_gettime(CLOCK_MONOTONIC, &end);

		printf("Benchmark_faster Time Print...!\n");
		printf("start time  :: %ld\n" , start.tv_sec);
		printf("end time    :: %ld\n" , end.tv_sec);
		printf("Differ time :: %ld\n" , end.tv_sec - start.tv_sec);

	return 0;
}