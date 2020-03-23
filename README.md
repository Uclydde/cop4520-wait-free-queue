# cop4520-wait-free-queue
Re-implementation of a wait-free queue as fast as fetch-and-add for COP 4520 (Multicore Programming, Spring 2020) at the University of Central Florida.

To compile, please run the following in Ubuntu 18.04:

g++ -I mrlock/src/ mrlock-queue.cpp mrlock/src/strategy/*.o --std=c++11 -pthread 

To run, please execute:
./a.out
