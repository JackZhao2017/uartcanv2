#include "UartCanRingbuffer.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

#define  lock pthread_mutex_lock(&ringBufferMutex);
#define  unlock pthread_mutex_unlock(&ringBufferMutex);

namespace uartcan{
UartCanRingbuffer::UartCanRingbuffer(void)
{
	ringBuffer=NULL;
	pthread_mutex_init(&ringBufferMutex,NULL);
	printf("%s\n",__func__ );
}

UartCanRingbuffer::~UartCanRingbuffer(void)
{
	pthread_mutex_destroy(&ringBufferMutex);
	printf("%s\n",__func__);
}



int  UartCanRingbuffer::RingbufferInit(int size)
{
	if(size<=0)
		return 0;

	ringBuffer=(RINGBUFFER*)malloc(sizeof(RINGBUFFER));
	if(ringBuffer==NULL)
		return 0;

	memset(ringBuffer,0,sizeof(RINGBUFFER));

	ringBuffer->data=(unsigned char *)malloc(size);
	if(ringBuffer->data==NULL)
		return 0;

	ringBuffer->size=size;
	ringBuffer->mark=size-1;

	return 1;
}

void UartCanRingbuffer::RingbufferRelease(void)
{
	free(ringBuffer->data);
	memset(ringBuffer,0,sizeof(RINGBUFFER));
}

int  UartCanRingbuffer::PutdataTobuffer(unsigned char *buf,int len)
{
	int i=0;
	if(ringBuffer==NULL)
		return 0;
	if(ringBuffer->data==NULL)
		return 0;
	if(len<=0)
		return 0;


	lock;
	if(len>ringBuffer->size - ringBuffer->num){
		len=ringBuffer->size - ringBuffer->num;
	}
	unlock;

	if(ringBuffer->putaddr+len>ringBuffer->size){
		memcpy(&ringBuffer->data[ringBuffer->putaddr],buf,ringBuffer->size-ringBuffer->putaddr);
		memcpy(&ringBuffer->data[0],&buf[ringBuffer->size-ringBuffer->putaddr],ringBuffer->putaddr+len-ringBuffer->size);
	}else{
		memcpy(&ringBuffer->data[ringBuffer->putaddr],buf,len);
	}
	lock;
	ringBuffer->num	  +=len;
	ringBuffer->putaddr=(ringBuffer->putaddr+len)%ringBuffer->size; 
	unlock;

	return 1;
}
int  UartCanRingbuffer::GetbufferHeardata(unsigned char *buf,int len)
{
	if(ringBuffer==NULL)
		return 0;
	if(ringBuffer->data==NULL)
		return 0;

	lock;
	if(len>ringBuffer->num){
		len=ringBuffer->num;
	}
	unlock;
	// printf("%s %d\n",__func__,len );
	if(ringBuffer->getaddr+len>ringBuffer->mark){
		memcpy(buf,&ringBuffer->data[ringBuffer->getaddr],ringBuffer->size-ringBuffer->getaddr);
		memcpy(&buf[ringBuffer->size-ringBuffer->getaddr],&ringBuffer->data[0],ringBuffer->getaddr+len-ringBuffer->size);
	}else{
		memcpy(buf,&ringBuffer->data[ringBuffer->getaddr],len);
	}
	return len;
}


int  UartCanRingbuffer::AddBufferHearaddr(int len)
{
    //printf("%s %d \n",__func__,len );
	lock;
	ringBuffer->num-=len;
	ringBuffer->getaddr=(ringBuffer->getaddr+len)%ringBuffer->size; 
	unlock;
	return 1;
}

int  UartCanRingbuffer::GetdataFrombuffer(unsigned char *buf,int len)
{
	if(ringBuffer==NULL)
		return 0;
	if(ringBuffer->data==NULL)
		return 0;
	//uartcan_sem_wait(&uartcan_parse_sem);
	lock;
	if(len>ringBuffer->num){
		unlock;
		return 0;
	}
	unlock;

	if(ringBuffer->getaddr+len>ringBuffer->size){
		memcpy(buf,&ringBuffer->data[ringBuffer->getaddr],ringBuffer->size-ringBuffer->getaddr);
		memcpy(&buf[ringBuffer->size-ringBuffer->getaddr],&ringBuffer->data[0],ringBuffer->getaddr+len-ringBuffer->size);
	}else{
		memcpy(buf,&ringBuffer->data[ringBuffer->putaddr],len);
	}

	lock;
	ringBuffer->num-=len;
	ringBuffer->getaddr=(ringBuffer->getaddr+len)%ringBuffer->size; 
	unlock;
	return 1;

}

int UartCanRingbuffer::CheckCharBuffer(unsigned char sync)
{
	int i=0;
	int sum=ringBuffer->num;

	for(i=0;i<sum;i++){

		if(ringBuffer->data[ringBuffer->getaddr]==sync){

			AddBufferHearaddr(1);		
			return 1;
		}

	}	
	
	return 0;
}

void UartCanRingbuffer::CleanRingbuffer(void)
{
	 ringBuffer->getaddr=0;
	 ringBuffer->putaddr=0;
	 ringBuffer->num=0;
}

};
