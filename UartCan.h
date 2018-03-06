#ifndef _UARTCAN_H_
#define _UARTCAN_H_

#include <termios.h>
#include <unistd.h>

#include "UartCanReceive.h"
#include "UartCanSend.h"

namespace uartcan{


class UartCan
{

public:
	UartCan();
	~UartCan();

	int  UartCanInit(const char *dev,int bandrate);


	void UartCanRelease(void);

	void PutAdasinfoToUartCan(const ADAS_INFO *adasInfo);

	void GetUartCanSpeedresult(float *speed);	

	void setCallback(Callback callback);

private:

	int  	 devfd;
	int      devInit(const char *dev,int bandrate);
	void     devRelease(void);	
	
	speed_t  baudrate_map(unsigned long b);
	
};

};


#endif