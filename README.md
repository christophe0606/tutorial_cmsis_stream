# README

CMSIS Stream examples that can be tested on Windows / Linux / Mac.

You must first install the cmsis-stream python package:

```python
pip install cmsis-stream
```

Then, link the example you want to run in the `CMakeLists.txt`

Build and run.

Look at the Python script `create.py` and C++ `AppNodes.hpp` file in each example folder for more explanations.

`stream` is providing the CMSIS-Stream C++ definitions that are OS independent. They define the datatypes like Event, StreamNode etc ...

`posix_runtime` is a possible and simple implementation of the CMSIS Stream event system for this tutorial.
