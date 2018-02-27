#include "UartCan.h"

#include <stdio.h>
#include <stdlib.h>

using namespace uartcan;


void SpeedCallback(float *speed)
{
	printf("%s %.1f\n",__func__,*speed);
}

int main(int argc, char const *argv[])
{
	UartCan uarcan;
	uarcan.UartCanInit("/dev/ttymxc1",115200);
	uarcan.setCallback(SpeedCallback);
	ADAS_INFO adasInfo;
	while(1)
	{
		float speed;

		uarcan.PutAdasinfoToUartCan((void*)&adasInfo);
		printf("%.1f\n",speed );
		sleep(1);
	}
	uarcan.UartCanRelease();
	return 0;
}
