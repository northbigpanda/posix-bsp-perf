#ifndef __MSG_HEAD_H__
#define __MSG_HEAD_H__

#include <any>
#include <functional>
#include <memory>

namespace bsp_sockets
{
class EventLoop;
static constexpr int MSG_HEAD_LENGTH = 8;
static constexpr int MSG_LENGTH_LIMIT = 65536 - MSG_HEAD_LENGTH;

using msgHead = struct msgHead
{
    int cmd_id;
    int length;
};

using msgTask = struct msgTask
{
    msgTask(std::function<void(std::shared_ptr<EventLoop>, std::any)> task, std::any args):
        task(task), args(args) {}
    std::function<void(std::shared_ptr<EventLoop>, std::any)> task;
    std::any args;
 };//for NEW_TASK, 向sub-thread下发待执行任务

//for accepter communicate with connections
//for give task to sub-thread
using queueMsg = struct queueMsg
{
    enum class MSG_TYPE
    {
        NEW_CONN,
        STOP_THD,
        NEW_TASK,
    } cmd_type;

    int connection_fd;//for NEW_CONN, 向sub-thread下发新连接
    std::shared_ptr<msgTask> task_handle{nullptr};
};

}


#endif
