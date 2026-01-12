/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/


#include <cstdint>
#include "app_config.hpp"
#include "stream_platform_config.hpp"
#include "cg_enums.h"
#include "StreamNode.hpp"
#include "cstream_node.h"
#include "IdentifiedNode.hpp"
#include "EventQueue.hpp"
#include "GenericNodes.hpp"
#include "AppNodes.hpp"
#include "scheduler.h"

#if !defined(CHECKERROR)
#define CHECKERROR       if (cgStaticError < 0) \
       {\
         goto errorHandling;\
       }

#endif


#if !defined(CG_BEFORE_ITERATION)
#define CG_BEFORE_ITERATION
#endif 

#if !defined(CG_AFTER_ITERATION)
#define CG_AFTER_ITERATION
#endif 

#if !defined(CG_BEFORE_SCHEDULE)
#define CG_BEFORE_SCHEDULE
#endif

#if !defined(CG_AFTER_SCHEDULE)
#define CG_AFTER_SCHEDULE
#endif

#if !defined(CG_BEFORE_BUFFER)
#define CG_BEFORE_BUFFER
#endif

#if !defined(CG_BEFORE_FIFO_BUFFERS)
#define CG_BEFORE_FIFO_BUFFERS
#endif

#if !defined(CG_BEFORE_FIFO_INIT)
#define CG_BEFORE_FIFO_INIT
#endif

#if !defined(CG_BEFORE_NODE_INIT)
#define CG_BEFORE_NODE_INIT
#endif

#if !defined(CG_AFTER_INCLUDES)
#define CG_AFTER_INCLUDES
#endif

#if !defined(CG_BEFORE_SCHEDULER_FUNCTION)
#define CG_BEFORE_SCHEDULER_FUNCTION
#endif

#if !defined(CG_BEFORE_NODE_EXECUTION)
#define CG_BEFORE_NODE_EXECUTION(ID)
#endif

#if !defined(CG_AFTER_NODE_EXECUTION)
#define CG_AFTER_NODE_EXECUTION(ID)
#endif





CG_AFTER_INCLUDES


using namespace arm_cmsis_stream;

/*

Description of the scheduling. 

*/
static uint8_t schedule[3]=
{ 
2,0,1,
};

/*

Internal ID identification for the nodes

*/
#define PROCESSING_INTERNAL_ID 0
#define SINK_INTERNAL_ID 1
#define SOURCE_INTERNAL_ID 2



/***********

Node identification

************/
static CStreamNode identifiedNodes[STREAMNB_IDENTIFIED_NODES]={0};

CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 128
#define FIFOSIZE1 128

#define BUFFERSIZE0 512
CG_BEFORE_BUFFER
uint8_t streambuf0[BUFFERSIZE0]={0};

#define BUFFERSIZE1 512
CG_BEFORE_BUFFER
uint8_t streambuf1[BUFFERSIZE1]={0};


typedef struct {
FIFO<float,FIFOSIZE0,1,0> *fifo0;
FIFO<float,FIFOSIZE1,1,0> *fifo1;
} fifos_t;

typedef struct {
    ProcessingNode<float,128,float,128> *processing;
    Sink<float,128> *sink;
    Source<float,128> *source;
} nodes_t;


static fifos_t fifos={0};

static nodes_t nodes={0};

CStreamNode* get_scheduler_node(int32_t nodeID)
{
    if (nodeID >= STREAMNB_IDENTIFIED_NODES)
    {
        return(nullptr);
    }
    if (nodeID < 0)
    {
        return(nullptr);
    }
    return(&identifiedNodes[nodeID]);
}

int init_scheduler(void *evtQueue_)
{
    EventQueue *evtQueue = reinterpret_cast<EventQueue *>(evtQueue_);

    CG_BEFORE_FIFO_INIT;
    fifos.fifo0 = new (std::nothrow) FIFO<float,FIFOSIZE0,1,0>(streambuf0);
    if (fifos.fifo0==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo1 = new (std::nothrow) FIFO<float,FIFOSIZE1,1,0>(streambuf1);
    if (fifos.fifo1==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }

    CG_BEFORE_NODE_INIT;
    cg_status initError;

    nodes.processing = new (std::nothrow) ProcessingNode<float,128,float,128>(*(fifos.fifo0),*(fifos.fifo1));
    if (nodes.processing==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[STREAMPROCESSING_ID]=createStreamNode(*nodes.processing);
    nodes.processing->setID(STREAMPROCESSING_ID);

    nodes.sink = new (std::nothrow) Sink<float,128>(*(fifos.fifo1));
    if (nodes.sink==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[STREAMSINK_ID]=createStreamNode(*nodes.sink);
    nodes.sink->setID(STREAMSINK_ID);

    nodes.source = new (std::nothrow) Source<float,128>(*(fifos.fifo0));
    if (nodes.source==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[STREAMSOURCE_ID]=createStreamNode(*nodes.source);
    nodes.source->setID(STREAMSOURCE_ID);


/* Subscribe nodes for the event system*/

    initError = CG_SUCCESS;
    initError = nodes.processing->init();
    if (initError != CG_SUCCESS)
        return(initError);
    
    initError = nodes.sink->init();
    if (initError != CG_SUCCESS)
        return(initError);
    
    initError = nodes.source->init();
    if (initError != CG_SUCCESS)
        return(initError);
    
   


    return(CG_SUCCESS);

}

void free_scheduler()
{
    if (fifos.fifo0!=NULL)
    {
       delete fifos.fifo0;
    }
    if (fifos.fifo1!=NULL)
    {
       delete fifos.fifo1;
    }

    if (nodes.processing!=NULL)
    {
        delete nodes.processing;
    }
    if (nodes.sink!=NULL)
    {
        delete nodes.sink;
    }
    if (nodes.source!=NULL)
    {
        delete nodes.source;
    }
}

void reset_fifos_scheduler(int all)
{
    if (fifos.fifo0!=NULL)
    {
       fifos.fifo0->reset();
    }
    if (fifos.fifo1!=NULL)
    {
       fifos.fifo1->reset();
    }
   // Buffers are set to zero too
   if (all)
   {
       std::fill_n(streambuf0, BUFFERSIZE0, (uint8_t)0);
       std::fill_n(streambuf1, BUFFERSIZE1, (uint8_t)0);
   }
}


CG_BEFORE_SCHEDULER_FUNCTION
uint32_t scheduler(int *error)
{
    int cgStaticError=0;
    uint32_t nbSchedule=0;





    /* Run several schedule iterations */
    CG_BEFORE_SCHEDULE;
    while(cgStaticError==0)
    {
        /* Run a schedule iteration */
        CG_BEFORE_ITERATION;
        unsigned long id=0;
        for(; id < 3; id++)
        {
            CG_BEFORE_NODE_EXECUTION(schedule[id]);
            switch(schedule[id])
            {
                case 0:
                {
                    
                   cgStaticError = nodes.processing->run();
                }
                break;

                case 1:
                {
                    
                   cgStaticError = nodes.sink->run();
                }
                break;

                case 2:
                {
                    
                   cgStaticError = nodes.source->run();
                }
                break;

                default:
                break;
            }
            CG_AFTER_NODE_EXECUTION(schedule[id]);
                        CHECKERROR;
        }
       CG_AFTER_ITERATION;
       nbSchedule++;
    }
errorHandling:
    CG_AFTER_SCHEDULE;
    *error=cgStaticError;
    return(nbSchedule);
    
}
