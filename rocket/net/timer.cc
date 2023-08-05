#include <sys/timerfd.h>
#include <string.h>
#include "rocket/net/timer.h"
#include "rocket/common/log.h"
#include "rocket/common/util.h"

namespace rocket{
    Timer::Timer():FdEvent(){
        m_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
        DEBUGLOG("timer fd=%d", m_fd);

        listen(FdEvent::TriggerEvent::IN_EVENT, std::bind(&Timer::onTimer,this));
    }

    Timer::~Timer(){

    }

    void Timer::onTimer(){
        DEBUGLOG("ontimer");
        char buf[8];
        while(1){
            if((read(m_fd,buf,8)==-1)&&errno == EAGAIN){
                break;
            }
        }

        int64_t now = getNowMs();

        std::vector<TimerEvent::s_ptr> tmps;
        std::vector<std::pair<int64_t,std::function<void()>>> tasks;

        ScopeMutex<Mutex> lock(m_mutex);

        auto it = m_pending_events.begin();
        
        for(it = m_pending_events.begin(); it != m_pending_events.end(); ++it){
            if((*it).first <= now){
                if(!(*it).second->isCancled()){
                    tmps.push_back((*it).second);
                    tasks.emplace_back((*it).second->getArriveTime(), (*it).second->getCallBack());
                }
            }else{
                break;
            }
        }

        m_pending_events.erase(m_pending_events.begin(),it);

        lock.unlock();

        for(auto it : tmps){
            if(it->isRepeated()){
                it->resetArriveTime();
                addTimerEvent(it);
            }
        }

        resetArriveTime();

        for(auto it : tasks){
            if(it.second){
                it.second();
            }
        }

    }


    void Timer::resetArriveTime(){
        ScopeMutex<Mutex> lock(m_mutex);
        auto tmp = m_pending_events;
        lock.unlock();

        if(tmp.size() == 0){
            return;
        }

        int64_t now = getNowMs();

        auto it = tmp.begin();
        int64_t interval = 0;
        if(it->second->getArriveTime()>now){
            interval = it->second->getArriveTime() - now;
        }else{
            interval = 100;
        }

        timespec ts;

        memset(&ts, 0, sizeof(ts));

    }
}