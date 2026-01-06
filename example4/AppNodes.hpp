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

class Sink : public StreamNode
{

public:
    enum selector {selNumber=0};
    static std::array<uint16_t,1> selectors;

    Sink() : StreamNode() {};

    void processTensor(TensorPtr<float> v)
    {
        std::cout << "Sink received value: " << std::endl;
        // To access the data we need to lock the protected buffer. We can use a write lock or a
        // read lock.
        // Here we only want to read it:

        bool lockError;
        v.lock_shared(lockError, [](const Tensor<float> &tensor)
        {
            const float *buf = tensor.buffer();
            for (size_t i = 0; i < 5; i++)
            {
                std::cout << buf[i] << std::endl;
            }
            std::cout << std::endl;
        });
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

