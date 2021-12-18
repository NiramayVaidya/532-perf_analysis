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

#define DEBUG 1
#define INFO 1

#define NUM_TX 5
#define NUM_ITEMS 5
#define THRESHOLD 1
#define LEVEL 5

#define NAIVE_METHOD 1

typedef struct equivalence_class_indexes {
	int start;
	int end;
	int offset;
} equivalence_class_indexes;

#endif /* PTHREADSALGO_H */
