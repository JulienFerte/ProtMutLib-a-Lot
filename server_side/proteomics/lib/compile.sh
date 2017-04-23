#!/bin/bash

rm libmmcifchains.a
g++ -c libmmcifchains.cpp -L./cifparse-obj-v7.105-prod-src/lib/ -lmmcif
ar rvs libmmcifchains.a libmmcifchains.o
ranlib libmmcifchains.a
rm libmmcifchains.o

rm libmmtfparser.a
gcc -c mmtf_parser.c -lmsgpackc
ar rvs libmmtfparser.a mmtf_parser.o
ranlib libmmtfparser.a
rm mmtf_parser.o
