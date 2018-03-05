#ifndef _UARTCANPARSE_H_
#define _UARTCANPARSE_H_

#include "UartCanThread.h"
#include "UartCanDecode.h"
#include "UartCanRingbuffer.h"
#include "UartCanSemaphore.h"
namespace uartcan{


class UartCanParse:public UartCanThread
{

public:
	UartCanParse();
	~UartCanParse();

	static void * 	UartCanParsefunc(void *arg);
	
	int 			Init(void);
	int 			GetSpeedParseResult(float *speed);
	void 			SetSpeedParseCallback(Callback callback);
	void 			Release(void);	

	void  			PostParse(unsigned char *buf,int len);
	void            WaitParse(void);

private:
	UartCanDecode       *decode;
	UartCanRingbuffer   *Ringbuffer;
	uartcan_semaphore_t uartcan_parse_sem;
};


};

#endif
