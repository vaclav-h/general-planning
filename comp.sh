#!/bin/bash
gcc -std=gnu99 -Wall -O3 -pedantic -c -o problem.o problem.c
g++ -std=c++11 -Wall -O3 -pedantic -o planner planner.cpp problem.o
