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
#include "params.h"

typedef struct equivalence_class_indexes {
	int start;
	int end;
	int offset;
} equivalence_class_indexes;

void pthreadsalgo_run(entry *dataset);

#endif /* PTHREADSALGO_H */
