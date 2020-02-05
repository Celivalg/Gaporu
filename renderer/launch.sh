#!/bin/bash

rm out.bmp 
make clean 
make unit-test && ./unit-test && feh out.bmp
