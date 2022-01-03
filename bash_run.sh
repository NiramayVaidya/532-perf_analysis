nvcc -std=c++11 dataset.cpp seqalgo_parallel.cu -o seqalgo
#g++ -std=c++11 dataset.cpp seqalgo.cpp -o seqalgo
for x in 1 2 3
do
    nvprof ./seqalgo
done
ls -la frequent_itemsets_naive.txt frequent_itemsets.txt
#./seqalgo > log
#vim log
