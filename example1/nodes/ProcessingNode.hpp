#pragma once

#include "cg_enums.h"
#include "StreamNode.hpp"
#include "GenericNodes.hpp"

using namespace arm_cmsis_stream;

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