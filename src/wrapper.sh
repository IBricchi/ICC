#!/bin/bash
set -e

echo "Compiling to MIPS..."
cat $2 | ./bin/compiler 2> bin/log.txt 1> $4

echo "Compiling finished!"
