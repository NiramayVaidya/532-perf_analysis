#include "dataset.h"

#define DEBUG 0

#define NUM_TX 1000
#define NUM_ITEMS 30

int main(int argc, char *argv[]) {
	entry *data = (entry *) malloc(NUM_TX * sizeof(entry));

	generate_dataset(data, NUM_TX, NUM_ITEMS);

#if DEBUG
	print_dataset(data);
#endif

	save_dataset(data);

	free(data);

	return 0;
}
