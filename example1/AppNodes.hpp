#pragma once

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

// Generic template declaration for processing node
template <typename IN, int inputSize,
          typename OUT, int outputSize>
class ProcessingNode;

/*

Specialized implementation of the template with the constraints:
IN == OUT
inputSize == outputSize == inputOutputSize

If the Python is generating a node where those constraints are
not respected, the C++ will not typecheck and build.

There are a remaining degrees of freedom :
The datatype (for input and output) can be chosen : IN
The number of samples (produced and consumed)
can be chosen : inputOutputSize

As consequence, the template specialization still use some
arguments IN and inputOutputSize.

*/
template <typename IN, int inputOutputSize>
class ProcessingNode<IN, inputOutputSize, IN, inputOutputSize> : public GenericNode<IN, inputOutputSize, IN, inputOutputSize>
{
public:
    /* Constructor needs the input and output FIFOs */
    ProcessingNode(FIFOBase<IN> &src,
                   FIFOBase<IN> &dst) : GenericNode<IN, inputOutputSize,
                                                    IN, inputOutputSize>(src, dst) {};

    /*
       Node processing
       1 is added to the input
    */
    int run() final
    {
        std::cout << "ProcessingNode" << std::endl;
        IN *a = this->getReadBuffer();
        IN *b = this->getWriteBuffer();
        for (int i = 0; i < inputOutputSize; i++)
        {
            b[i] = a[i] + 1;
        }
        return (CG_SUCCESS);
    };
};

template <typename IN, int inputSize>
class Sink : public GenericSink<IN, inputSize>
{
    static constexpr int maxNbRuns_ = 5;

public:
    Sink(FIFOBase<IN> &src) : GenericSink<IN, inputSize>(src) {};

    // Implementation of the node.
    // The input is printed on stdout.
    // So this node will build only if the IN
    // datatype has an implementation of << to
    // be printed on stdout
    // This node stops the execution of the graph after nb runs
    // because for debug we do not want an infinite output
    int run() final
    {
        IN *b = this->getReadBuffer();
        std::cout << "Sink" << std::endl;
        // Only the first 5 samples are printed
        for (int i = 0; i < 5; i++)
        {
            std::cout << (int)b[i] << std::endl;
        }
        std::cout << std::endl;

        nbRun_++;
        if (nbRun_ >= maxNbRuns_)
        {
            return (CG_STOP_SCHEDULER);
        }
        return (CG_SUCCESS);
    };

protected:
    int nbRun_ = 0;
};

// Generic template declaration for processing node
template <typename IN, int inputSize,
          typename OUT, int outputSize>
class Adder;

template <typename IN, int inputOutputSize>
class Adder<IN, inputOutputSize, IN, inputOutputSize> : public GenericNode<IN, inputOutputSize, IN, inputOutputSize>
{
public:
    /* Constructor needs the input and output FIFOs */
    Adder(FIFOBase<IN> &src,
          FIFOBase<IN> &dst,int increment) : GenericNode<IN, inputOutputSize,
                                           IN, inputOutputSize>(src, dst), increment_(increment) {};

    int run() final
    {
        IN *a = this->getReadBuffer();
        IN *b = this->getWriteBuffer();
        for (int i = 0; i < inputOutputSize; i++)
        {
            b[i] = a[i] + increment_;
        }
        return (CG_SUCCESS);
    };
protected:
    int increment_;
};