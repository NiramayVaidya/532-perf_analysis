#### Steps to compile and run-

```
g++ dataset.cpp seqalgo.cpp -o seqalgo
./seqalgo
```

## Launch on GPU
```
nvcc -std=c++11 dataset.cpp seqalgo.cu -o seqalgo
./seqalgo
```
