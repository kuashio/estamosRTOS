// estamosRTOS
// by Kuashio
//
// Example application with 3 tasks.

// Uncomment the following line to enable debug mode
// #define ESTAMOSRTOS_DEBUG

#include "estamosRTOS.h"

volatile int shared=0, count=0;
estamosRTOS_mutex my_mutex=0;

#define ITERATIONS 5

void task1(){
  volatile int i=0, j=0;
	while(1){
		i=ITERATIONS;
		while(estamosRTOS_mutex_lock(&my_mutex)){
			//TODO: Yield();
			estamosRTOS_yield();
		}
		while(i--){	
			j=(i%10)?j:j+1;
			shared=i+j;
		}
		estamosRTOS_mutex_unlock(&my_mutex);
		i=20;
		i=30+j;
		j=40-i*j;
		i=j+50;
		j=60-i*7;
		i=j+70;
		i=80;
	}
}

void task2(){
  volatile int i=0, j=0;
	while(1){
		i=20;
		i=30+j;
		j=40-i*j;
		i=j+50;
		j=60-i*7;
		i=j+70;
		i=80;
		i=ITERATIONS;
		while(estamosRTOS_mutex_lock(&my_mutex)){
			//TODO: Yield();
			estamosRTOS_yield();
		}
		while(i--){	
			j=(i%10)?j:j+3;
			shared=i+j;
		}
		estamosRTOS_mutex_unlock(&my_mutex);
	}
}

void task3(){
  volatile int i=0, j=0;
	while(1){
		
		i=ITERATIONS;
		if(!estamosRTOS_mutex_lock(&my_mutex)){
			i=ITERATIONS;
			while(i--){	
				j=(i%10)?j:j+5;
				shared=i+j;
			}
			estamosRTOS_mutex_unlock(&my_mutex);
		}		
		i=20;
		i=30+j;
		j=40-i*j;
		i=j+50;
		j=60-i*7;
		i=j+70;
		i=80;
	}
}


int main(){
	task t1,t2,t3;
	
	estamosRTOS_init();
	
	estamosRTOS_add_task(&t1,task1);
	estamosRTOS_add_task(&t2,task2);
	estamosRTOS_add_task(&t3,task3);
	
	estamosRTOS_start();

  // This point should never be reached!!!!
  // Write your error notifications/assertions here.
	//
	// Execution may reach this point if the programmer
	// misuses the functions, for example, failing to 
	// call estamosRTOS_start(), which does not return
	
  while(1){
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright © 2017-2018, Eduardo Corpeño
////////////////////////////////////////////////////////////////////////////////////////////////////

