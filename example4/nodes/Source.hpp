#pragma once

#include "cg_enums.h"
#include "StreamNode.hpp"
#include "GenericNodes.hpp"

#include <thread>
#include <chrono>

using namespace arm_cmsis_stream;

// Template describes the numpert of IO ports.
// For each port there is the datatype and the number of samples
// Inputs are listed first, then outputs.
template <typename OUT, int outputSize>
class Source : public GenericSource<OUT, outputSize>
{
public:
    static constexpr int msWaitTime = 20;
    Source(FIFOBase<OUT> &dst) : GenericSource<OUT, outputSize>(dst) {};

    int run() final
    {
        // Simulate waiting for data to be available from external driver
        std::this_thread::sleep_for(std::chrono::milliseconds(msWaitTime));
        
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