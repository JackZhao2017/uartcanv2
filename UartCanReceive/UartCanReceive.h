#ifndef _UARTCANRECEIVE_H
#define _UARTCANRECEIVE_H

#include "UartCanParse.h"
#include "UartCanDecode.h"
#include "UartCanRead.h"

namespace uartcan{

class UartCanReceive
{
public:
	UartCanReceive();
	~UartCanReceive();


	int  Init(int fd);
	int  GetReceiveSpeed(float *speed);
	void Release(void);
	void SetReceiveSpeedCallback(Callback callback);
private:

	UartCanRead  *uartRead;
	
};

};


#endif