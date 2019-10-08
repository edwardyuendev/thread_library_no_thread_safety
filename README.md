# User Mode Thread Library without Thread Synchronization or Thread Safety
A more advanced version of this is here: https://github.com/edwardyuendev/thread_library_with_threadsync

The application is intended to serve as a library that implemented a basic thread system for Linux. These threads run virtually in parallel in the same address space of a single process but each with its own context consisting of CPU registers and a stack.

The implementation is essentially my own implementations of core functions from pthread.h, where I wrote function implementations from scratch. I designed the underlying data structures to keep track of threads with a Thread Control Block, a data structure to store the blocks and a scheduler that pre-emptively switches between threads.

# The Goals of this Project were to:
1. understand the idea of threads
2. implement independent, parallel execution within a single process
3. understand alarms and signals within Linux
4. improve proficiency with the C++ language

# Functions implemented
- pthread_create()
- pthread_exit()
- pthread_self()
- scheduler, alarm, and wrapper functions

# How To Run
1. Make sure both the Makefile and pthread.cpp is in the same directory
2. Type "make" to generate the object files
3. Include pthread.h in your application and you are now able to utilize basic threads

# Restrictions: 
- The thread library was designed to work specifically on Linux (Fedora OS)
- The thread library includes pointer mangling for the purposes of running/testing the thread library on lab Linux computers
