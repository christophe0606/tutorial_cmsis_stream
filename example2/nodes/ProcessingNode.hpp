#pragma once

#include "cg_enums.h"
#include "StreamNode.hpp"
#include "GenericNodes.hpp"

using namespace arm_cmsis_stream;

// Generic template declaration for sink node
template <typename IN, int inputSize>
class ProcessingNode: public GenericSink<IN, inputSize>
{
    // To limit the number of execution in the tutorial (otherwise it executes forever)
    static constexpr int maxNbRuns_ = 5;

public:
    /* Constructor needs the input and output FIFOs */
    ProcessingNode(FIFOBase<IN> &src, EventQueue *queue) : GenericSink<IN, inputSize>(src),ev0(queue) {};

    /*
       Node processing
       1 is added to the input
    */
    int run() final
    {
        std::cout << "ProcessingNode" << std::endl;
        IN *a = this->getReadBuffer();
        
        // Send a Value event in an asynchronous way. Event is sent on output event port 0
        // The event is put into the event queue and processed later by the event thread
        // The event thread always has lower priority than the dataflow thread
        // Note that sending this event in a synchronous way is not advised here
        // since the dataflow should not be blocked by event processing and we do not
        // know how much time it will take to process the event in the receiving nodes.
        ev0.sendAsync(kNormalPriority,kValue,(int16_t)(a[0]+1)); // Send a async Value event to all subscribers

        nbRun_++;
        if (nbRun_ >= maxNbRuns_)
        {
            return (CG_STOP_SCHEDULER);
        }

        return (CG_SUCCESS);
    };

    // At init, when a new node is connected to an output port this function is called.
    // Here we only have one output event port (port 0) so we do not check dstPort
    // to know in which output queue the node must be put.
    // By default, we put all connected nodes to output queue ev0.
    void subscribe(int outputPort,StreamNode &dst,int dstPort) final
    {
        ev0.subscribe(dst,dstPort);
    }

protected:
   EventOutput ev0;
   int nbRun_ = 0;

};