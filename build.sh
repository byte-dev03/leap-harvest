#!/bin/sh

gcc -Wall -I./include *.c -o game -lraylib -lm -ltmx -lxml2 -lz
