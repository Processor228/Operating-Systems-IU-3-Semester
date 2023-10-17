#!/bin/bash

gcc scheduler.c -o sch
gcc worker.c -o wor

./sch "$1"
