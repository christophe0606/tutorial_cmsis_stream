#pragma once

#include "StreamNode.hpp"

using namespace arm_cmsis_stream;

// Template describes the numpert of IO ports.
// For each port there is the datatype and the number of samples
// Inputs are listed first, then outputs.
template <typename OUT, int outputSize>
class Source : public GenericSource<OUT, outputSize>
{
public:
    Source(FIFOBase<OUT> &dst) : GenericSource<OUT, outputSize>(dst) {};

    int run() final
    {
        // Get output buffer. The code does not see the FIFO.
        // It only sees a buffer to write to.
        // Call to getWriteBuffer is modifying the FIFO state so should be called only once
        // in the run() method.
        OUT *b = this->getWriteBuffer();

        std::cout << "Source" << std::endl;
        for (int i = 0; i < outputSize; i++)
        {
            b[i] = (OUT)i;
        }
        return (CG_SUCCESS);
    };
};

// Generic template declaration for sink node
template <typename IN, int inputSize>
class ProcessingNode: public GenericSink<IN, inputSize>
{
    // To limit the number of execution in the tutorial (otherwise it executes forever)
    static constexpr int maxNbRuns_ = 5;

public:
    /* Constructor needs the input and output FIFOs */
    ProcessingNode(FIFOBase<IN> &src) : GenericSink<IN, inputSize>(src) {};

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

class Sink : public StreamNode
{

public:
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
        if (evt.event_id == kValue)
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

