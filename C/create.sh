#1/bin/bash

gcc -fopenmp -lm -fPIC -shared -o transf.so transform.c
rm -f ../Python/transf.so
cp ./transf.so ../Python/