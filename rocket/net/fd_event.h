#ifndef ROCKET_NET_FDEVENT_H
#define ROCKET_NET_FDEVENT_H
#include <functional>
#include <sys/epoll.h>

namespace rocket
{
    class FdEvent{
        public:
            enum class TriggerEvent{
                IN_EVENT = EPOLLIN,
                OUT_EVENT = EPOLLOUT,
            };
            
            FdEvent(int fd);

            FdEvent();

            ~FdEvent();

            void setNonBlock();

            std::function<void()>handler(TriggerEvent envent_type);

            void listen(TriggerEvent event_type,std::function<void()> callback);

            void cancle(TriggerEvent event_type);

            int getFd() const{
                return m_fd;
            }

        protected:
            int m_fd{-1};

            epoll_event m_listen_event;
            std::function<void()> m_read_callback;
            std::function<void()> m_wirte_callback;

    };
    
} // namespace rocket


#endif