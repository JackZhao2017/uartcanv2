#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UartCanParse.h"
#include "UartCanDecode.h"
#include "UartCanSemaphore.h"

namespace uartcan{

UartCanParse::UartCanParse()
{
	uartcan_semaphore_init(&uartcan_parse_sem,0);
	printf("%s\n",__func__ );
}
UartCanParse::~UartCanParse()
{	
	uartcan_semaphore_destroy(&uartcan_parse_sem);
	printf("%s\n",__func__ );
}
void UartCanParse::PostParse(unsigned char *buf,int len)
{
	if(len==0){
		decode->setSpeedValid(0);
		return ;
	}
	Ringbuffer->PutdataTobuffer(buf,len);
	uartcan_semaphore_post(&uartcan_parse_sem);
}
void UartCanParse::WaitParse(void)
{
	uartcan_semaphore_wait(&uartcan_parse_sem);
}

void *UartCanParse::UartCanParsefunc(void *arg)
{
	UartCanParse  *ptr=(UartCanParse*)arg;
	unsigned char buf[256];
	int getlen=0,i=0;
	memset(buf,0,sizeof(buf));
	while(1)
	{
		int syncindex=0;
		unsigned char type=0;

		ptr->WaitParse();
		
		getlen=ptr->Ringbuffer->GetbufferHeardata(buf,256);

		for(syncindex=0;syncindex<getlen;syncindex++)
		{
     		 if(!(ptr->decode->DetectMsgSync(buf,syncindex))){
     		 	continue;
     		 }
     		 
			 if(++syncindex>=getlen){
			 	syncindex-=1;
			 	break;
			 }

			 if(ptr->decode->DetectMsgType(&buf[syncindex],&type)){

			 	if(syncindex+type-1>=getlen){
			 		syncindex-=1;
			 		break;
			 	}
			 	ptr->decode->ParseMsg(&buf[syncindex],type);
			 	syncindex+=type-1;
			 }			 
	    }

		ptr->Ringbuffer->AddBufferHearaddr(syncindex);
				
		if(ptr->thread_exit){
			break;
		}	  	
	}
	
	printf("%s  exit\n",__func__ );	
	pthread_exit(0);
}

int UartCanParse::GetSpeedParseResult(float *speed)
{
	return decode->GetCanSpeed(speed);
}
void UartCanParse::SetSpeedParseCallback(Callback callback)
{
	return decode->setSpeedCallback(callback);
}

int UartCanParse::Init(void)
{
	Ringbuffer=new UartCanRingbuffer;
    decode = new UartCanDecode;
    Ringbuffer->RingbufferInit(256);
	launchThread(UartCanParsefunc,this);
}

void UartCanParse::Release(void)
{
	thread_exit=1;
	delete decode;
	delete Ringbuffer;
	uartcan_sem_post();
	exitThread();
	printf("%s\n",__func__ );
}

};