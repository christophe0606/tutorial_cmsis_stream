#include "init.hpp"
#include <iostream>
#include <cstdint>
#include <thread>
#include <chrono>

extern "C"
{
#include "cg_enums.h"
#include "cstream_node.h"
#include "scheduler.h"
}

#include "custom.hpp"
#include "StreamNode.hpp"
#include "EventQueue.hpp"
#include "StreamNode.hpp"
#include "cg_queue.hpp"
#include "posix_thread.hpp"

std::pmr::synchronized_pool_resource pool;

PlatformThread *cg_eventThread = nullptr;
bool cg_eventThreadDone = false;

bool app_handler(int src_node_id, void *data, arm_cmsis_stream::Event &&evt)
{
    printf("App received event with id %d from node %d\n", evt.event_id, src_node_id);
    return true;
}

int start_example()
{
    MyQueue *queue = new MyQueue();
    arm_cmsis_stream::EventQueue::cg_eventQueue = queue;
    arm_cmsis_stream::EventQueue::cg_eventQueue->setHandler(nullptr, app_handler);

    PosixThread t([]
                  {
                      std::cout << "Event thread started!" << std::endl;
                      // If we are done with the scheduling, we exit the thread
                      arm_cmsis_stream::EventQueue::cg_eventQueue->execute();
                      std::cout << "Event thread quitted!" << std::endl;
                  });

    queue->setThread(&t);
    t.setPriority(ThreadPriority::High);
    t.start();
    t.waitUntilStarted(); // Wait until the thread is ready to process events

    // Init may generate events in the queue so the event thread should already be started.
    int error = init_scheduler();
    if (error != CG_SUCCESS)
    {
        std::cout << "Error during scheduler initialization : " << error << std::endl;
        arm_cmsis_stream::EventQueue::cg_eventQueue->end();
        t.join();
        return error;
    }

    PosixThread ts([&error]
                  {
                      std::cout << "Scheduler thread started!" << std::endl;
                      uint32_t nb = scheduler(&error);
                      if ((error != CG_SUCCESS) && (error != CG_STOP_SCHEDULER))
                      {
                          std::cout << "Error during scheduler execution : " << error << std::endl;
                          std::cout << "Scheduler executed " << nb << " iterations" << std::endl;
                      }
                      else
                      {
                          std::cout << "Scheduler executed " << nb << " iterations" << std::endl;
                      }
                      std::cout << "Scheduler thread quitted!" << std::endl;
                  });

    ts.setPriority(ThreadPriority::RealTime);
    ts.start();
    ts.waitUntilStarted(); // Wait until the thread is ready to process events


    ts.join();
    // In a pure event graph application, the dataflow part may stop (because there is no dataflow)
    // and the event part would continue to run.
    // Here we are forcing the event part to stop by asking the event queue to stop processing./
    // Otherwise, the tutorial would never end.
    // In a real application if we want to stop the event graph we should call the event queue end function from somewhere else.
    // We add a pause before stopping event processing just for the tutorial to process some additional remaining events
    arm_cmsis_stream::EventQueue::cg_eventQueue->end();

    // Wait for event queue to finish
    t.join();

    free_scheduler();

    delete arm_cmsis_stream::EventQueue::cg_eventQueue;
    arm_cmsis_stream::EventQueue::cg_eventQueue = nullptr;
    return (error);
}