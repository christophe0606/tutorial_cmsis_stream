#pragma once

#include "cg_enums.h"
#include "StreamNode.hpp"
#include "GenericNodes.hpp"

using namespace arm_cmsis_stream;

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