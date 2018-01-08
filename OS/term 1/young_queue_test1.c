/*
 *  young_queue_test1.c
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

int main(void){

		// enqueue data over the queue size to test
		enqueue(0); enqueue(1);
		enqueue(2); enqueue(3);
		enqueue(4); enqueue(5);
		enqueue(6); enqueue(7);
		enqueue(8); enqueue(9);
		enqueue(10); enqueue(11);
		enqueue(12); enqueue(13);
		enqueue(14); enqueue(15);

	return 0;
}
