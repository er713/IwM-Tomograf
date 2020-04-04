#1/bin/bash

gcc -Wall -fopenmp -lm -fPIC -shared -o transf.so transform.c
rm -f ../Python/transf.so
cp ./transf.so ../Python/