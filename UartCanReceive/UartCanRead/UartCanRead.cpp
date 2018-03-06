#include "UartCanRead.h"
#include "UartCanRingbuffer.h"
#include "UartCanParse.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h> 
#include <sys/timerfd.h>
#include <signal.h>
#include <fcntl.h> 
#include <unistd.h>
#include <errno.h> 
#include <sys/time.h>
#include <errno.h>


namespace uartcan{
#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)


static struct itimerval g_timevalue; 
extern UartCanParse *g_uartParse;

uint64_t tot_exp = 0;

UartCanRead::UartCanRead()
{
	 g_dev_fd=-1;
	 printf("%s\n",__func__ );
}
UartCanRead::~UartCanRead()
{
	 printf("%s\n",__func__ );
}

void UartCanRead::timerfd_handler(int fd)
{
    uint64_t exp = 0;
    
    read(fd, &exp, sizeof(uint64_t)); 
    tot_exp += exp;
    printf("read: %llu, total: %llu\n", (unsigned long long)exp, (unsigned long long)tot_exp);

    return;
}

int UartCanRead::epoll_add_fd(int fd)
{
    int ret;
    struct epoll_event event;

    memset(&event, 0, sizeof(event));
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;

    ret = epoll_ctl(g_epfd, EPOLL_CTL_ADD, fd, &event);
    if(ret < 0) {
        printf("epoll_ctl Add fd:%d error, Error:[%d:%s]\n", fd, errno, strerror(errno));
        return -1;
    }
    printf("epoll add fd:%d--->%d success\n", fd, g_epfd);
    return 0;    
}
int UartCanRead::timerfd_init()
{
    int tmfd;
    int ret;
    struct itimerspec new_value;

    new_value.it_value.tv_sec = 1;
    new_value.it_value.tv_nsec = 0;
    new_value.it_interval.tv_sec = 1;
    new_value.it_interval.tv_nsec = 0;
    
    tmfd = timerfd_create(CLOCK_REALTIME, 0);
    if (tmfd < 0) {
        printf("timerfd_create error, Error:[%d:%s]\n", errno, strerror(errno));
        return -1;
    }

    ret = timerfd_settime(tmfd, 0, &new_value, NULL);
    if (ret < 0) {
        printf("timerfd_settime error, Error:[%d:%s]\n", errno, strerror(errno));
        close(tmfd);
        return -1;
    }

    if (epoll_add_fd(tmfd)) {
        close(tmfd);
        return -1;
    }
    g_tim_fd = tmfd;
    return 0;
}
int UartCanRead::epollfd_init(void)
{
    int epfd;

    epfd = epoll_create(256); 
    if (epfd < 0) {
        printf("epoll_create error, Error:[%d:%s]\n", errno, strerror(errno));
        return -1;
    }
    g_epfd = epfd;
    printf("epoll fd:%d create success\n", epfd);

    return epfd;
}

void *UartCanRead::UartCanReadfunc(void *arg)
{
  UartCanRead* ptr = reinterpret_cast<UartCanRead*>(arg);
  int i=0;
  int fd_cnt = 0;
  int sfd;
  struct epoll_event events[256];    
  unsigned char buf[1024];
  memset(events, 0, sizeof(events)); 
	while(1)
	{
    fd_cnt = epoll_wait(ptr->g_epfd, events, 256, -1); 
    for(i = 0; i < fd_cnt; i++) 
    {   
            sfd = events[i].data.fd;
            if(events[i].events & EPOLLIN) 
            {   
                if (sfd == ptr->g_tim_fd) 
                {
                      ptr->timerfd_handler(sfd); 
                }else{
                            int _s=read(sfd,buf,sizeof(buf)-1);  
                            int j=0;     
                            for(j=0;j<_s;j++)
                            {
                                printf("0x%x ",buf[j]);
                            }
                            printf("\n");  
                            g_uartParse->PostParse(buf,_s);
                }   
            }   
    } 
		if(ptr->thread_exit){
			  break;
		}
	}
	printf("%s  exit\n",__func__ );
	pthread_exit(0);
  return NULL;
}
int UartCanRead::Init(int fd)
{
	   g_dev_fd=fd;
     epollfd_init();
     epoll_add_fd(fd);
     pthread_create(&thread_pid,
                   NULL,
                   UartCanReadfunc,
                   this);
    return 1;
}

void UartCanRead::Release(void)
{
	   thread_exit=1;
     timerfd_init();
     if(pthread_join(thread_pid, NULL) != 0) {
        printf("%s faild\n",__func__ );
     }
}
};