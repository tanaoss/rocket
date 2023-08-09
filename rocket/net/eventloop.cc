#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <string.h>
#include "rocket/net/eventloop.h"
#include "rocket/common/log.h"
#include "rocket/common/util.h"
namespace rocket {
    static thread_local EventLoop*  t_current_eventloop = nullptr;
    static int g_epoll_max_timeout = 10000;
    static int g_epoll_max_events = 10;

    EventLoop::EventLoop() {
    if (t_current_eventloop != NULL) {
        ERRORLOG("failed to create event loop, this thread has created event loop");
        exit(0);
    }
    m_thread_id = getThreadId();

    m_epoll_fd = epoll_create(10);

    if (m_epoll_fd == -1) {
        ERRORLOG("failed to create event loop, epoll_create error, error info[%d]", errno);
        exit(0);
    }

    initWakaUpFdEvent();
    initTimer();

    INFOLOG("succ create event loop in thread %d", m_thread_id);
    t_current_eventloop = this;
    }
}
