#!/bin/bash


gcc scheduler_sjf.c -o sjf
gcc worker.c -o wor

./sjf "$1"
