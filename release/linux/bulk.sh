#!/bin/bash

for f in ../../samples/*.jpg
do
	./teojpg.sh -i $f -w -f4 -z MO6 -d BM16 -g 1.3 -s 1.2 -t 1.2
done
