#pragma once

#include "cg_enums.h"
#include "StreamNode.hpp"
#include "GenericNodes.hpp"

using namespace arm_cmsis_stream;

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