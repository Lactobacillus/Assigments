/*
 * QuickSort.S
 *
 *      Author : Cho Youngwoo
 *  Student ID : 2014210085
 *
 */

#include <stdio.h>
#include "QuickSort.h"

int data1[] = {

	#include "input.dat"
};

int data2[] = {

	#include "input.dat"
};

int check(int * data1, int * data2, int size);
void PrintData(int * data, int size);

int main(){

	int dataSize = sizeof(data1) / sizeof(int);

		BubbleSort(data1, dataSize);
		PrintData(data1, dataSize);
		QuickSort(data2, 0, dataSize - 1);
		PrintData(data2, dataSize);

		if (check(data1, data2, dataSize) == 1){
			printf("incorrect!\n");
		}
		else{
			printf("correct!\n");
		}

	return 0;
}

int check(int * data1, int * data2, int size){

	int i;
	int sum = 0;

		for(i = 0; i < size; i++){
			if (data1[i] != data2[i]){
				return 1;
			}
		}

	return 0;
}

void PrintData(int * data, int size){

	int i;

		for(i = 0; i < size; i++){
			printf("%d\t", data[i]);

			if(i % 16 == 15){
				printf("\n");
			}
	}
}
