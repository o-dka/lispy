#!/bin/bash
cc -std=c99 -Wall lisp.c mpc.c -g -ledit -lm -o lisp
