#ifndef _UARTCANREAD_H_
#define _UARTCANREAD_H_


#include "UartCanThread.h"
#include "UartCanCrc8.h"
namespace uartcan{

class UartCanRead:public UartCanThread
{
public:
	UartCanRead();
	~UartCanRead();


	int Init(int fd);
	void Release(void);

	static void *UartCanReadfunc(void *arg);
	
private:
	int dev_fd;
	
};

}

#endif


