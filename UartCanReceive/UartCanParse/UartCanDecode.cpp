#include "UartCanDecode.h"
#include "UartCanRingbuffer.h"



namespace uartcan{

#define   CANSPEED    0x04
#define   CANMSG      0x11 
#define   SYSMSG      0x07
#define   DECODESYNC  0x55

UartCanDecode::UartCanDecode()
{
	isSpeedValid=0;
	carSpeed=0;
	printf("%s\n",__func__ );
}

UartCanDecode::~UartCanDecode()
{
	printf("%s\n",__func__ );
}

int UartCanDecode::DetectMsgSync(unsigned char *buf,int index)
{
	int i=0;
	if(buf==NULL)
		return 0;
	if(buf[index]==DECODESYNC){
		//printf("%s %x\n",__func__,buf[index] );
		return 1;
	}
	// printf("%s %x\n",__func__,buf[index] );
	return 0;
}

int UartCanDecode::DetectMsgType(unsigned char *buf,unsigned char *type)
{
	int ret=0;
	if(buf==NULL)
		return 0;
	*type=buf[0];
	switch(*type){
			case CANMSG:
			case SYSMSG:	
			case CANSPEED:
				 //printf("%s type %x\n",__func__,*type);	
				 ret=1;
				 break;
			default:
				 ret=0;
				 break;
	}
	return ret;
}

int UartCanDecode::DetectMsgCrc8(unsigned char *data,int start,int len)
{
	return detectcrc8(data,start,len);
}

int UartCanDecode::ParseMsg(unsigned char *data,int type)
{
	int ret=0;

	if(DetectMsgCrc8(data,0,type))
	{
		switch(type)
		{
			case CANSPEED:
				 if(data[1]==0){
				 	    isSpeedValid=1;
						SAE_J1939_Speed(data,2);
				 }else{
				 		isSpeedValid=0;
				 }	
				 ret=1;			 
				 break;
			default:
				break;
		}
	}
	return ret;

}

void UartCanDecode::SAE_J1939_Speed(unsigned char *buf,int index)
{
	carSpeed=(float)buf[index];
	if(mSpeedCallback!=NULL)
	{
		mSpeedCallback(&carSpeed);
	}
}

int  UartCanDecode::GetCanSpeed(float *speed)
{
	*speed=carSpeed;
	return isSpeedValid;
}

void UartCanDecode::setSpeedValid(int val)
{
	isSpeedValid=val;
}

};


