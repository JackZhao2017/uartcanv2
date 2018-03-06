#include "UartCanSend.h"
#include "UartCanThread.h"
#include "UartCanEncode.h"

#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>

#define   ADASCANID    0x18FB05E8
namespace uartcan{


UartCanSend::UartCanSend()
{	
	dev_fd=-1;
	printf("UartCanSend()\n" );
}

UartCanSend::~UartCanSend()
{
	printf("~UartCanSend()\n" );
}

void UartCanSend::StartCanSendOne(const ADAS_INFO *adasinfo)
{
	uartcan_lock();
	memcpy(&Adas_Info,adasinfo,sizeof(Adas_Info));
	uartcan_unlock();
	uartcan_sem_post();
}

void UartCanSend::WaitCanSend(ADAS_INFO *adasinfo)
{
	uartcan_sem_wait();
	uartcan_lock();
	memcpy(adasinfo,&Adas_Info,sizeof(Adas_Info));
	uartcan_unlock();
}

void *UartCanSend::UartCanSendfunc(void *arg)
{	
	int i=0;
	// UartCanSend  *ptr=(UartCanSend *)arg;
	UartCanSend* ptr = reinterpret_cast<UartCanSend*>(arg);
	
	while(1)
	{
		ptr->WaitCanSend(&ptr->Adas_Info);

		if(ptr->thread_exit){
			break;
		}	

		ptr->convertadasInfo2uartstream(&ptr->Adas_Info,ADASCANID,ptr->stream);

		write(ptr->dev_fd,ptr->stream,CANTXMSGSIZE);
		
		// for(i=0;i<CANTXMSGSIZE;i++)
		// {
		// 	printf("0x%x ",ptr->stream[i] );
		// }
		// printf("\n");
		
		if(ptr->thread_exit){
			break;
		}
	}	
	printf("%s exit\n",__func__ );
	pthread_exit(0);
	return NULL;
}

void UartCanSend::Init(int fd)
{
	dev_fd=fd;
	launchThread(UartCanSendfunc,this);
}
void UartCanSend::Release(void)
{
	thread_exit=1;
	uartcan_sem_post();
	exitThread();
	printf("%s\n",__func__ );
}

};

