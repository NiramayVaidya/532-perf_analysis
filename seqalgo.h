#ifndef SEQALGO_H
#define SEQALGO_H

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
#define DEBUG 0
#define INFO 0

#define NUM_TX 1000
#define NUM_ITEMS 30
#define THRESHOLD 10
#define LEVEL 30

#define NAIVE_METHOD 1
#endif

void seqalgo_run(entry *db);

#endif /* SEQALGO_H */
