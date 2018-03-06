#include "UartCanParse.h"
#include "UartCanDecode.h"
#include "UartCanReceive.h"
#include "UartCanRingbuffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace uartcan{

UartCanParse *g_uartParse;

UartCanReceive::UartCanReceive()
{
	
}

UartCanReceive::~UartCanReceive()
{

}

int UartCanReceive::Init(int fd)
{
	if(fd<=0){
		return 0;
	}
	g_uartParse =  new UartCanParse;

	g_uartParse->Init();

	uartRead  =  new UartCanRead;

	uartRead ->Init(fd);
	
	return 1;
}

int UartCanReceive::GetReceiveSpeed(float *speed)
{
	return g_uartParse->GetSpeedParseResult(speed);
}
void UartCanReceive::SetReceiveSpeedCallback(Callback callback)
{
	g_uartParse->SetSpeedParseCallback(callback);
}

void UartCanReceive::Release(void)
{
	uartRead->Release();
	g_uartParse->Release();
	delete uartRead;
	delete g_uartParse;

}

};