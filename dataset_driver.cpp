#include "dataset.h"

#define DEBUG 1

#define NUM_TX 2000
#define NUM_ITEMS 50

#define LOAD_DB 0

using namespace std;

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

	free(data);

	return 0;
}
