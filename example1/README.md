# README

Simple dataflow graph with 3 nodes : source -> processing -> sink
The sink prints some data on stdout

Run `create.py` from this folder to regenerate the C++ CMSIS Stream for the graph.
Edit `create.py` to change the graph.

`AppNodes.hpp` includes the templates for all the nodes used in the graph.
One could include every existing nodes in the project (even if not used in the graph) and let the compiler get rid of the definitions that are not used. But it would slow the compilation.
It is better to put in `AppNodes.hpp` only the nodes that are used.

The Python script `create.py` could even generate automatically the file `AppNodes.hpp` and include only the templates that are needed for the graph.

