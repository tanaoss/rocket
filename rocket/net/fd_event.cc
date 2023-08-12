#include <string.h>
#include <fcntl.h>
#include "rocket/common/log.h"
#include "rocket/net/fd_event.h"

namespace rocket{

    FdEvent::FdEvent(int fd_):m_fd(fd_){  
        memset(&m_listen_event,0,sizeof(m_listen_event));
    }

    FdEvent::FdEvent(){  
        memset(&m_listen_event,0,sizeof(m_listen_event));
    }

    FdEvent::~FdEvent(){  

    }

    std::function<void()> FdEvent::handler(TriggerEvent event_type){
        if(event_type == TriggerEvent::IN_EVENT){
            return m_read_callback;
        }else{
            return m_wirte_callback;
        }
    }

    void FdEvent::listen(TriggerEvent event_type, std::function<void()> callback){
        if(event_type == TriggerEvent::IN_EVENT){
            m_listen_event.events |= EPOLLIN;
            m_read_callback=callback;
        }else{
            m_listen_event.events |= EPOLLOUT;
            m_wirte_callback = callback;
        }
         m_listen_event.data.ptr = this;
    }

    void FdEvent::setNonBlock(){

        int flag=fcntl(m_fd,F_GETFL,0);
        if(flag & O_NONBLOCK){
            return;
        }

        fcntl(m_fd,F_SETFL, flag | O_NONBLOCK);
        return ;
    }
}