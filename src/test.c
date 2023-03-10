#include <stdio.h>
#include <time.h>


void test(void){
  DLOG("this is function test\r\n");
}

int add(int a, int b)
{
	time_t previous_time, current_time = time(NULL);
	previous_time = current_time;
	DLOG("in add: %d\r\n", a+b);
	
	while(1){
		time_t current_time = time(NULL);
		if(current_time - previous_time >= 1){
			DLOG("current_time: %d\r\n", (int)current_time);
			previous_time = current_time;
		}

	}


    return a+b;
}
