/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/

#ifndef SCHEDULER_H_ 
#define SCHEDULER_H_


#include <stdint.h>

#ifdef   __cplusplus
extern "C"
{
#endif

#include "cstream_node.h"


/* Node identifiers */
#define STREAMNB_IDENTIFIED_NODES 3
#define STREAMPROCESSING_ID 0
#define STREAMSOURCE_ID 1
#define STREAMSINK_ID 2

/* Selectors global identifiers */
#define SEL_NUMBER_ID 100 

extern CStreamNode* get_scheduler_node(int32_t nodeID);

extern int init_scheduler(void *evtQueue_);
extern void free_scheduler();
extern uint32_t scheduler(int *error);
extern void reset_fifos_scheduler(int all);

#ifdef   __cplusplus
}
#endif

#endif

