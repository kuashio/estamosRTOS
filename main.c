// estamosRTOS
// by Kuashio
//
// Example application with 3 tasks.

// Uncomment the following line to enable debug mode
// #define ESTAMOSRTOS_DEBUG

#include "estamosRTOS.h"

volatile int count1=0, count2=0, count3=0;

void task1(){
  volatile int i=0, j=0;
	while(1){
		i++;
		j=(i%100)?j:j+1;
		count1=i+j;
	}
}

void task2(){
  volatile int i=0, j=0;
	while(1){
		i++;
		j=(i%100)?j:j+1;
		count2=i+j;
	}
}

void task3(){
  volatile int i=0, j=0;
	while(1){
		i++;
		j=(i%100)?j:j+1;
		count3=i+j;
	}
}


int main(){
	task t1,t2,t3;
	
	estamosRTOS_init();
	
	estamosRTOS_add_task(&t1,task1);
	estamosRTOS_add_task(&t2,task2);
	estamosRTOS_add_task(&t3,task3);
	
	estamosRTOS_start();

  // La Kawandeep!!!
  // Write your error notifications/assertions here.
	//
	// Execution may reach this point if the programmer
	// misuses the functions, for example, failing to 
	// call estamosRTOS_start(), which does not return
	
  while(1){
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright © 2017, Eduardo Corpeño
////////////////////////////////////////////////////////////////////////////////////////////////////

