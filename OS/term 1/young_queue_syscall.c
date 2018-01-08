/*
 *  young_queue_syscall.c
 *  Operating System 2016
 *  Author : Cho Youngwoo
 *  2016.04.01
 */

#include <linux/linkage.h>
#include <linux/kernel.h>

// variable for queue
// size of queue is 10
int front = 0;
int rear = -1;
int young_queue[10];

asmlinkage int sys_young_enqueue(int n){

	int idx;

		if (rear == 10){
			// check queue is full
			// if it is full, return -1
			printk("========== queue is full...! ==========\n");

			return -1;
		}

		for (idx = front; idx <= rear; idx++){
			// check duplicated elements
			// if it is duplicated, return -1
			if (young_queue[idx] == n){

				printk("========== there is same element...! ==========\n");

				return -1;
			}
		}
		// enqueue the given data
		rear = rear + 1;
		young_queue[rear] = n;

		printk("========== enqueue : %d...! ==========\n", n);

		printk("========== queue : ");
		for (idx = front; idx <= rear; idx++){
			// print all elements in queue
			printk("%d ", young_queue[idx]);
		}
		printk("==========\n");

	return n;
}

asmlinkage int sys_young_dequeue(void){

	int idx;
	int data;

		if (rear < front){
			// check the queue is empty
			// if it is empty, return -1
			printk("========== queue is empty...! ==========\n");

			return -1;
		}
		else{
			// if queue is not empty, then dequeue data
			data = young_queue[front];
			front = front + 1;

			printk("========== dequeue : %d...! ==========\n", data);
		}

		printk("========== queue : ");
		for (idx = front; idx <= rear; idx++){
			// print all elements in queue
			printk("%d ", young_queue[idx]);
		}
		printk("==========\n");

	return data;
}
