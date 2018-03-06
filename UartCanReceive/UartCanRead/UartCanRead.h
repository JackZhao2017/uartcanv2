#ifndef _UARTCANREAD_H_
#define _UARTCANREAD_H_


#include "UartCanThread.h"
#include <sys/epoll.h>
#include <signal.h>
#include <pthread.h>

namespace uartcan{

class UartCanRead//:public UartCanThread
{
public:
	UartCanRead();
	~UartCanRead();
	int Init(int fd);
	void Release(void);

	static void *UartCanReadfunc(void *arg);
	
private:
	pthread_t 		thread_pid;
	int        		thread_exit;

	int g_dev_fd;
	int g_tim_fd;
	int g_epfd;
	int epollfd_init(void);
	int timerfd_init();
	int epoll_add_fd(int fd);
	void timerfd_handler(int fd);
};

};

#endif


