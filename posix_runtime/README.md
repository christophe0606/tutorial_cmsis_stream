# README

It is using C++ standard library for most of the features.

Thread priority control is platform dependent and the code is different for Windows, Linux and Mac.

It is handled in `posix_thread.cpp` and the difference is hidden for the CMSIS Stream event runtime.
