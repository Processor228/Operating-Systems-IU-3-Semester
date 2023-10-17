#!/bin/bash

gcc scheduler_rr.c -o sch_rr
gcc worker.c -o wor

./sch_rr "$1"