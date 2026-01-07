#pragma once

#include "cg_enums.h"
#include "StreamNode.hpp"
#include "GenericNodes.hpp"

using namespace arm_cmsis_stream;

class Sink : public StreamNode
{

public:
    enum selector {selNumber=0};
    static std::array<uint16_t,1> selectors;

    Sink() : StreamNode() {};

    void processValue(int32_t v)
    {
        std::cout << "Sink received value: " << v << std::endl;
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
            if (evt.wellFormed<int32_t>())
            {
                evt.apply<int32_t>(&Sink::processValue, *this);
            }
           
        }
    }

   
protected:
    int nbRun_ = 0;
};

