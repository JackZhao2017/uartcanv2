
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>


#include "UartCan.h"
#include "UartCanSend.h"
#include "UartCanReceive.h"



namespace uartcan{

UartCanSend 	*cansend; 
UartCanReceive  *canreceive;

UartCan::UartCan()
{
	devfd=-1;
}
UartCan::~UartCan()
{

}

speed_t UartCan::baudrate_map(unsigned long b)
{
    speed_t retval;
    switch(b)
    {
        case 9600:
            retval = B9600;
            break;

        case 19200:
            retval = B19200;
            break;

        case 38400:
            retval = B38400;
            break;

        case 57600:
            retval = B57600;
            break;

        case 115200:
            retval = B115200;
            break;
        default:
        	retval =0;
        	break;
    }
    return(retval);
}

int UartCan::devInit(const char *dev,int  baudrate)
{
	struct termios options;
	int fd=-1;

	if(fd<0){
	   fd = open(dev, O_RDWR | O_NOCTTY |O_NONBLOCK);
	   if (fd <0) {
			goto err;
		}
	}
	fcntl(fd, F_SETFL, 0);
	tcgetattr(fd, &options);

	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag &= ~PARODD;
	options.c_cflag |= CS8;
	options.c_cflag &= ~CRTSCTS;

	options.c_lflag &= ~(ICANON | IEXTEN | ISIG | ECHO| ECHONL);
	options.c_oflag &= ~OPOST;
	options.c_iflag &= ~(ICRNL | INPCK | ISTRIP | IXON | BRKINT );

	options.c_cc[VMIN] = 1;
	options.c_cc[VTIME] = 0;
	options.c_cflag |= (CLOCAL | CREAD);

	if(baudrate){
		cfsetispeed(&options, baudrate_map(baudrate));
		cfsetospeed(&options, baudrate_map(baudrate));
	}
	tcflush(fd ,TCIFLUSH);
	tcsetattr(fd, TCSANOW, &options);
	printf("<%s>UART %lu, %dbit, %dstop, %s, HW flow %s\n",__func__,baudrate,8,
	       (options.c_cflag & CSTOPB) ? 2 : 1,
	       (options.c_cflag & PARODD) ? "PARODD" : "PARENB",
	       (options.c_cflag & CRTSCTS) ? "enabled" : "disabled");
	return fd;
err:   	
	fd =-1;
	return -1;
}

void UartCan::devRelease(void)
{
	close(devfd);
	devfd=-1;
}

int UartCan::UartCanInit(const char *dev,int bandrate)
{
	cansend   =new UartCanSend;
	canreceive=new UartCanReceive;

	devfd	  =devInit(dev,bandrate);
	cansend->Init(devfd);
	canreceive->Init(devfd);
}



void UartCan::UartCanRelease(void)
{
	cansend->Release();
	canreceive->Release();
	devRelease();
	delete cansend;
	delete canreceive;
}

void UartCan::GetUartCanSpeedresult(float *speed)
{
	canreceive->GetReceiveSpeed(speed);
}

void  UartCan::setCallback(Callback callback) 
{
	canreceive->SetReceiveSpeedCallback(callback);
}

void UartCan::PutAdasinfoToUartCan(void *adasInfo)
{
	ADAS_INFO *info=(ADAS_INFO*)adasInfo;
	cansend->StartCanSendOne(info);
}

};





