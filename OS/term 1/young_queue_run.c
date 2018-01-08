/*
 *  young_queue_run.c
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

int dequeue(void){

	int data;

		// call sys_young_dequeue and get return value
		data = syscall(317);

		// return value is dequeued value if it is not -1
		printf("dequeue : %d\n", data);

	return 0;
}

int main(void){

		// test case
		enqueue(10);
		enqueue(20);
		dequeue();
		enqueue(30);
		enqueue(20);

	return 0;
}
