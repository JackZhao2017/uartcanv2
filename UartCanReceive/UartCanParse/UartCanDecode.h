#ifndef _UARTCANCODE_H_
#define _UARTCANCODE_H_


#include "UartCanRingbuffer.h"
#include "UartCanCrc8.h"

namespace uartcan{

typedef void (*Callback)(float* );

class UartCanDecode:public UartCanCrc8
{
	
public:
	UartCanDecode();
	~UartCanDecode();

	void  SAE_J1939_Speed(unsigned char *buf,int index);
	int   GetCanSpeed(float *speed);
	void  setSpeedValid(int val);
	void  setSpeedCallback(Callback callback){mSpeedCallback=callback;}


	int   DetectMsgSync(unsigned char *buf,int index );
	int   DetectMsgType(unsigned char *buf,unsigned char *type);
	int   DetectMsgCrc8(unsigned char *data,int start,int len);
	int   ParseMsg(unsigned char *buf,int type);
	
private:
	Callback mSpeedCallback;
	float    carSpeed;
	int      isSpeedValid;
	
};

};


#endif