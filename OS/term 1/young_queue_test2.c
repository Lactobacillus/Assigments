/*
 *  young_queue_test2.c
 *  Operating System 2016
 *  Author : Cho Youngwoo
 *  2016.04.01
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

int enqueue(int n){
	
	int data;

		// call sys_young_enqueue and get return value
		data = syscall(316, n);

		// if n == data, successfully enqueued
		printf("enqueue : %d, return : %d\n", n, data);

	return 0;
}

int main(int argc, char * argv[]){

		// enqueue semi random value
		// it may be address of argument
		enqueue(argv[1]);

	return 0;
}
