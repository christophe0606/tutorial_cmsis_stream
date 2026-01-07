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
    enum selector {selNumber=0};
    static std::array<uint16_t,1> selectors;
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

        // This will allocate memory with malloc
        // It is possible to provide a custom deleter to use another way to free the memory
        // It is also possible to provide a pointer to an existing memory area and the corresponding deleter
        // This class is otherwise similar to std::unique_ptr but the deleter does not appear in the
        // template parameters and so it can be used more easily in events.
        UniquePtr<IN> ptr(inputSize);

        // We write data into the unique pointer buffer
        for (int i = 0; i < inputSize; i++)
        {
            ptr[i] = a[i] + 1;
        }

        // Before exchanging this buffer with other nodes, it needs to be wrapped
        // in an object doing atomic reference counting.
        // It is similar to std::shared_ptr but the reference counting update are protected with a mutex.
        // The class ProtectedBuffer is generally used either for RawBuffer or Tensor so
        // aliases and convenience methods are provided to wrap the unique pointer in a protected buffer.

        // We create a 1D tensor with inputSize elements of type IN.
        // TensorPtr is an alias to a ProtectedBuffer<Tensor<IN>>

        TensorPtr<IN> t = TensorPtr<IN>::create_with((uint8_t)1,
                                                    cg_tensor_dims_t{inputSize},
                                                    std::move(ptr));

        
        
        // Send a Number event in an asynchronous way. Event is sent on output event port 0
        // The event is put into the event queue and processed later by the event thread
        // The event thread always has lower priority than the dataflow thread
        // Note that sending this event in a synchronous way is not advised here
        // since the dataflow should not be blocked by event processing and we do not
        // know how much time it will take to process the event in the receiving nodes.

        ev0.sendAsync(kNormalPriority,selectors[selNumber],std::move(t)); // Send a async Number event to all subscribers
        // If there is only one subscriber the event queue will move the event
        // If there are several subscribers, the event queue will clone the events
        // which will increase the ref count of the ProtectedBuffer but won't copy the UniquePtr data

        nbRun_++;
        if (nbRun_ >= maxNbRuns_)
        {
            // If some events are still in the event queue they will may not be processed.
            // As soon as the stream thread has quitted, the event queue processing is stopped.
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