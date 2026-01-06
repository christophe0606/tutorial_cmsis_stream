#include <iostream>
#include "init.hpp"
#include "cg_enums.h"

int main(int argc,char **argv)
{
    std::cout << "CMSIS Stream tutorial" << std::endl;
    int err = start_example();
    if ((err != CG_SUCCESS) && (err != CG_STOP_SCHEDULER))
    {
        switch(err)
        {
            case CG_BUFFER_UNDERFLOW:
                std::cout << "Error: Buffer underflow" << std::endl;
                break;
            case CG_BUFFER_OVERFLOW:
                std::cout << "Error: Buffer overflow" << std::endl;
                break;
            case CG_MEMORY_ALLOCATION_FAILURE:
                std::cout << "Error: Memory allocation failure" << std::endl;
                break;
            case CG_INIT_FAILURE:
                std::cout << "Error: Node initialization failure" << std::endl;
                break;
            case CG_OS_ERROR:
                std::cout << "Error: OS error" << std::endl;
                break;
            
            default:
                std::cout << "Error: Unknown error code " << err << std::endl;
        }
    }
    return 0;
}
