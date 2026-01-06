#include "init.hpp"
#include <iostream>
#include <cstdint>

extern "C" {
#include "cg_enums.h"
#include "cstream_node.h"
#include "scheduler.h"
}

int start_example()
{
    int error = init_scheduler();
    if (error != CG_SUCCESS)
    {
        std::cout << "Error during scheduler initialization : " << error << std::endl;
        return error;
    }

    uint32_t nb = scheduler(&error);
    if ((error != CG_SUCCESS) && (error != CG_STOP_SCHEDULER))
    {
        std::cout << "Error during scheduler execution : " << error << std::endl;
        std::cout << "Scheduler executed " << nb << " iterations" << std::endl;
        return error;
    }
    else
    {
        std::cout << "Scheduler executed " << nb << " iterations" << std::endl;
    }
    free_scheduler();
    return(CG_SUCCESS);
}