
#include "UartCan.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h> 
#include <sys/timerfd.h>
#include <signal.h>
#include <fcntl.h> 
#include <unistd.h>
#include <errno.h> 
#include <sys/time.h>
#include <errno.h>
#include <termios.h>

using namespace uartcan;


void SpeedCallback(float *speed)
{
	printf("%s %.1f\n",__func__,*speed);
}


int main(int argc, char const *argv[])
{
	int count=0;
	UartCan *uartcan=new UartCan;
	uartcan->UartCanInit("/dev/ttymxc1",115200);
	while(1)
	{
		sleep(1);
		if(++count>10)
			break;
	}
	uartcan->UartCanRelease();
	return 0;
}
