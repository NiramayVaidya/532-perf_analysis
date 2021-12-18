#ifndef PTHREADSALGO_H
#define PTHREADSALGO_H

#include <pthread.h>

#include <vector>
#include <tuple>
#include <map>
#include <set>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <chrono>

#define DEBUG 0
#define INFO 0

#define NUM_TX 100
#define NUM_ITEMS 100
#define THRESHOLD 10
#define LEVEL 100

#define NAIVE_METHOD 1

typedef struct equivalence_class_indexes {
	int start;
	int end;
	int offset;
} equivalence_class_indexes;

#endif /* PTHREADSALGO_H */
