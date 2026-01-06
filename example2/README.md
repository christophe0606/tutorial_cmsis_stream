# README

Simple dataflow graph with 2 dataflow nodes and an event node: source -> processing ##> sink
The sink prints some data on stdout and receive the data through an event.
To simplify the example, the sink is only receving one float and not a buffer.
Buffer management is demonstrated in other examples

Run `create.py` from this folder to regenerate the C++ CMSIS Stream for the graph.
Edit `create.py` to change the graph.
