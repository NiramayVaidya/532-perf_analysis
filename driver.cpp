#include "seqalgo.h"
#include "pthreadsalgo.h"

#define LOAD_DB 0

string load_file = "dataset.txt";

int main(int argc, char *argv[]) {
	entry *data = (entry *) malloc(NUM_TX * sizeof(entry));

#if LOAD_DB
	load_dataset(data, NUM_TX, NUM_ITEMS, load_file);
#else
	generate_dataset(data, NUM_TX, NUM_ITEMS);
#endif

#if DEBUG
	print_dataset(data);
#endif

#if !LOAD_DB
	save_dataset(data);
#endif

	seqalgo_run(data);

	pthreadsalgo_run(data);

	free(data);

	return 0;
}
