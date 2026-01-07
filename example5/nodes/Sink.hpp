#pragma once

#include "cg_enums.h"
#include "StreamNode.hpp"
#include "GenericNodes.hpp"

using namespace arm_cmsis_stream;

class Sink : public StreamNode
{

public:
    enum selector
    {
        selNumber = 0
    };
    static std::array<uint16_t, 1> selectors;

    Sink() : StreamNode() {};

    void processTensor(TensorPtr<float> v)
    {
        std::cout << "Sink received value: " << std::endl;
        // To access the data we need to lock the protected buffer. We can use a write lock or a
        // read lock.
        // Here we only want to read it:

        bool lockError;
        v.lock(lockError, [](bool isShared, Tensor<float> &tensor)
               {
            float *buf = tensor.buffer();
            // If buffer is shared we cannot modify it inplace
            if (isShared)
            {
                 for (size_t i = 0; i < 5; i++)
                 {
                     std::cout << (buf[i] + 1) << std::endl;
                 }
            }
            else
            {
                // Inplace modification since we are the only owner of the buffer
                for (size_t i = 0; i < 5; i++)
                {
                    buf[i] = buf[i] + 1;
                }
                for (size_t i = 0; i < 5; i++)
                {
                    std::cout << buf[i] << std::endl;
                }
            }
            std::cout << std::endl; });
    }

    // When a new event is received on any of the input ports, this function is called.
    // dstPort is the port number receiving the event.
    // This function may be called from different thread but to simplify things
    // one could impose that only the event thread can call it.
    // So if an event must be sent to the node from another source (the application), it should go to
    // the event queue rather than use this function directly. It has an overhead but
    // simplifies the implementation of this function. It is always run from event thread and
    // since there is only one event thread in this design there is no concurrency issue.
    // A more complex design may use a thread pool to process events in parallel
    void processEvent(int dstPort, Event &&evt) final override
    {
        if (evt.event_id == selectors[selNumber])
        {
            if (evt.wellFormed<TensorPtr<float>>())
            {
                evt.apply<TensorPtr<float>>(&Sink::processTensor, *this);
            }
        }
    }

protected:
    int nbRun_ = 0;
};
