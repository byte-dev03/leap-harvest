#!/bin/sh

gcc -g -Wall -I./include *.c -o game -lraylib -lm -ltmx -lxml2 -lz
