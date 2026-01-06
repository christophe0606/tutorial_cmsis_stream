# Include definitions from the Python package
from cmsis_stream.cg.scheduler import GenericNode,GenericSink,GenericSource
from cmsis_stream.cg.scheduler import Graph,CType,F32,Configuration
from cmsis_stream.cg.scheduler.graphviz import Style

import subprocess

########################
### Define new types of Nodes here

class Source(GenericSource):
    """Source node producing data of a given type and length"""
    def __init__(self,name,theType,outLength):
        GenericSource.__init__(self,name)
        self.addOutput("o",theType,outLength)

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "Source"


class ProcessingNode(GenericNode):
    """Processing node with one input and one output of given types and lengths"""
    def __init__(self,name,theType,inLength,outLength):
        GenericNode.__init__(self,name)
        self.addInput("i",theType,inLength)
        self.addOutput("o",theType,outLength)

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "ProcessingNode"

class Sink(GenericSink):
    """Sink node consuming data of a given type and length"""
    def __init__(self,name,theType,inLength):
        GenericSink.__init__(self,name)
        self.addInput("i",theType,inLength)

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "Sink"
    
########################
### Creation of the graph

# Define a float datatype
floatType=CType(F32)

# Create the nodes with io datatype and number of sample produces / consumed
src=Source("source",floatType,128)
processing=ProcessingNode("processing",floatType,128,128)
sink=Sink("sink",floatType,128)

# Create a Graph object
the_graph = Graph()

# Connect the source to the processing node
the_graph.connect(src.o,processing.i)
# Connect the processing node to the sink
the_graph.connect(processing.o,sink.i)

#####################################
# Generation of the C++ wrapper code and initialization code
conf = Configuration()
# Don't use float32_t for float datatype but use the standard float (so that CMSIS-DSP is not needed)
conf.CMSISDSP = False
# Synchronous execution (asynchronous mode is deprecated)
conf.asynchronous = False
# Layout of the generated graph picture
conf.horizontal=True
# Generate ID and macros to identify the nodes from outside the graph
# Needed if the nodes must be controlled from outside the graph
conf.nodeIdentification = True
# Name of scheduler function
conf.schedName = "scheduler"
# Memory optimization of buffer for dataflow (require buffer to be aligned on 8 bytes)
conf.memoryOptimization = True
# Prefix to add before name of buffer. By default the names are buf0, buf1 but
# it can conflict with other variables in the project
conf.prefix = "stream"

# Compute the dataflow scheduling
scheduling = the_graph.computeSchedule(config=conf)

# Schedule length = number of states in generated scheduler state machine
# If this number is too big it generally means that some
# input / output lengths are too mismatched and may indicate and error in the graph
# description
print(f"Schedule length = {scheduling.scheduleLength}")
# Memory usage of the FIFOs : to check the effect of memory optimization
# (For some graphs, memory optimization is not possible and memory usage remains the same)
print(f"Memory usage {scheduling.memory} bytes")

# Generation of the files
# Generate the C++ state machine and initialization code in current directory
scheduling.ccode(".",conf)

# Optional : generate some json data
# It can be used by external tools. For instance, one coudlcould imagine a PC sending an event
# to a graph running on a board. In that case, the node ID and selector ID must be known
# The json is giving a mapping from string to ID.
# This mapping is graph dependent
scheduling.genJsonIdentification("./description",conf)
scheduling.genJsonSelectors("./description",conf)
scheduling.genJsonSelectorsInit("./description",conf)

# Generate hte graphviz file for visualization of the graph
myStyle = Style()
with open("./description/graph.dot","w") as f:
        scheduling.graphviz(f,style=myStyle)

# Optional : if graphviz tools installed you can generate a png or pdf
subprocess.run(["dot","-Tpng","./description/graph.dot","-o","./graph.png"])