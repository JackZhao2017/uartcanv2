#include "UartCanRead.h"
#include "UartCanRingbuffer.h"
#include "UartCanParse.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h> 
#include <fcntl.h> 
#include <unistd.h>
#include <errno.h> 


namespace uartcan{

extern UartCanParse *g_uartParse;

UartCanRead::UartCanRead()
{
	 dev_fd=-1;
	 printf("%s\n",__func__ );
}
UartCanRead::~UartCanRead()
{
	 printf("%s\n",__func__ );
}

void *UartCanRead::UartCanReadfunc(void *arg)
{
	// UartCanRead *ptr=(UartCanRead*)arg;
  UartCanRead* ptr = reinterpret_cast<UartCanRead*>(arg);
  int i=0;
  unsigned char buf[1024];
	int epfd    = epoll_create(256);

	struct epoll_event _ev;       //epoll结构填充   
  _ev.events  = EPOLLIN;         //初始关心事件为读  
  _ev.data.fd = ptr->dev_fd; 

  struct epoll_event revs[64]; 
  int timeout = 500;  
  int num = 0;  
  int done = 0; 

  epoll_ctl(epfd,EPOLL_CTL_ADD,ptr->dev_fd,&_ev);  

	while(1)
	{
    timeout=500;
		switch((num = epoll_wait(epfd,revs,64,timeout)))
		{
			      case -1:
				        perror("epoll_wait");
				        break;
            case 0:                  //返回0 ，表示监听超时  
                g_uartParse->PostParse(buf,0);
                printf("timeout\n");  
                break;
            default:
            	int i;
            	for(i=0;i<num;i++)
            	{
            		int fd=revs[i].data.fd;
 					      if(revs[i].events & EPOLLIN)  
            		{    
                			ssize_t _s = read(fd,buf,sizeof(buf)-1);  
                			if(_s > 0)  
               			  {  	
               			 //    		int j=0;
									          // for(j=0;j<_s;j++){
										         //    printf("0x%02x ",buf[j] );
									          // } 
                   //          printf("\n");
                            
                            g_uartParse->PostParse(buf,_s);
  									        
                    				_ev.events = EPOLLOUT | EPOLLET;  
                    				_ev.data.fd = fd;  
                    				epoll_ctl(epfd,EPOLL_CTL_ADD,ptr->dev_fd,&_ev);   
               				}  
               			  else if(_s == 0)  
                			{  
               					    printf("client:%d close\n",fd);  
                				    epoll_ctl(epfd,EPOLL_CTL_DEL,fd,NULL);  
  
                				    close(fd);  
               				}  
                			else  
               			  {  
                				    perror("read");  
               				}  
           			}  
            	}
            	break;
		}
		if(ptr->thread_exit){
			break;
		}
	}
	printf("%s  exit\n",__func__ );
	pthread_exit(0);
}
int UartCanRead::Init(int fd)
{
	dev_fd=fd;
	launchThread(UartCanReadfunc,this);
}

void UartCanRead::Release(void)
{
	thread_exit=1;
	uartcan_sem_post();
	exitThread();
	printf("%s\n",__func__ );
}
};