#!/bin/bash
gcc -std=gnu99 -Wall -pedantic -c -o problem.o problem.c
g++ -std=c++11 -Wall -pedantic -o planner planner.cpp problem.o
