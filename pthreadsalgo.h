#ifndef PTHREADSALGO_H
#define PTHREADSALGO_H

#include <pthread.h>
#include <unistd.h>
#include <sched.h>

#include <vector>
#include <tuple>
#include <map>
#include <set>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <chrono>

#include "dataset.h"

#ifndef FLAGS
#define FLAGS 1
#endif

#if FLAGS
// #define _GNU_SOURCE

#define DEBUG 0
#define INFO 0

#define NUM_TX 1000
#define NUM_ITEMS 30
#define THRESHOLD 10
#define LEVEL 30

#define USE_CPU_AFFINITY 0

#define NAIVE_METHOD 1
#endif

typedef struct equivalence_class_indexes {
	int start;
	int end;
	int offset;
} equivalence_class_indexes;

void pthreadsalgo_run(entry *dataset);

#endif /* PTHREADSALGO_H */
