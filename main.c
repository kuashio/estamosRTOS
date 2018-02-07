// estamosRTOS
// by Kuashio
//
// Example application with 3 tasks.

// Uncomment the following line to enable debug mode
// #define ESTAMOSRTOS_DEBUG
// ... or the following line to only enable logic analyzer signals
//#define ESTAMOSRTOS_LOGIC_ANALYZER

#include "estamosRTOS.h"

volatile int shared=0, count=0;
estamosRTOS_mutex my_mutex=0;
uint8_t t1=0,t2=0,t3=0,t4=0;

#define ITERATIONS 2

uint8_t iterations[4]={2,5,3,4};

void task1(){
  volatile unsigned int i=0, j=0, k=0;
	while(1){
		t1=t1?0:1;
		i=iterations[k++%4];
		while(estamosRTOS_mutex_lock(&my_mutex)){
			//TODO: Yield();
			estamosRTOS_yield();
		}
		while(i--){	
			j=(i%10)?j:j+1;
			shared=i+j;
			t1=t1?0:1;
		}
		estamosRTOS_mutex_unlock(&my_mutex);
		
		i=iterations[k++%4]*980;
		while(i--){
		  j=40-i*j;
		  j=60-i*7;
			t1=t1?0:1;
		}
	}
}

void task2(){
  volatile unsigned int i=0, j=0, k=0;
	while(1){
		t2=t2?0:1;
		i=20;
		i=30+j;
		j=40-i*j;
		i=j+50;
		j=60-i*7;
		i=j+70;
		i=80;
		i=iterations[k++%4];
		while(estamosRTOS_mutex_lock(&my_mutex)){
			estamosRTOS_yield();
		}
		while(i--){	
			j=(i%10)?j:j+3;
			shared=i+j;
		  t2=t2?0:1;
		}
		estamosRTOS_mutex_unlock(&my_mutex);
		
		i=iterations[k++%4]*230;
		while(i--){
		  j=40-i*j;
		  j=60-i*7;
			t2=t2?0:1;
		}
	}
}

void task3(){
  volatile unsigned int i=0, j=0, k=0;
	while(1){
		t3=t3?0:1;
		i=iterations[k++%4];
		if(!estamosRTOS_mutex_lock(&my_mutex)){
			while(i--){	
				t3=t3?0:1;
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

void task4(){
  volatile unsigned int i=0, j=0, k=0;
	while(1){
		t4=t4?0:1;
		i=iterations[k++%4];
		if(!estamosRTOS_mutex_lock(&my_mutex)){
			while(i--){	
				t4=t4?0:1;
				j=(i%10)?j:j+5;
				shared=i+j;
			}
			estamosRTOS_mutex_unlock(&my_mutex);
		}		
		i=20;
		i=20+j;
		j=20-i*7;
		i=j+70;
		i=20;
	}
}


int main(){
	task tsk1,tsk2,tsk3,tsk4;
	
	estamosRTOS_init();
	
	estamosRTOS_add_task(&tsk1,task1);
	estamosRTOS_add_task(&tsk2,task2);
	estamosRTOS_add_task(&tsk3,task3);
	estamosRTOS_add_task(&tsk4,task4);
	
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

