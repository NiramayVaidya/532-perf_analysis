nvcc -std=c++11 dataset.cpp seqalgo.cu -o seqalgo
./seqalgo > log
vim log
