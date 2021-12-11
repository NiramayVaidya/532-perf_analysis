#include "dataset.h"

int g_len;
int g_total_items;
// entry *db;
int cnt_1 = 0;

static int get_probabilistic_value() {
	int val = 0;
	int dist_len = 10000;
	int rand_val = rand() % dist_len;
	// printf("Rand value is : %d\n", rand_val);
	float chance = 0.5; // 80% chance to get 0
	if (rand_val > chance * dist_len) { 
		val = 1;
		cnt_1++;
	}
	return val;
}

void generate_dataset(entry *db, int len, int total_items) {
	g_len = len;
	g_total_items = total_items;
	// this fills up the database
	srand (time(NULL));
	for (int i = 0; i < len; i++) {
		db[i].txid = i;
		for (int j = 0; j < total_items;j++) {
			db[i].item_present[j] = get_probabilistic_value(); // TODO - some probabilistic function should give a value
		}
	}
}

void print_dataset(entry *db) {
	// first row
	for (int i = 0; i < g_total_items + 1; i++) {
		if (i == 0) {
			printf("TX ID\t");
		}
		else {
			printf("Item %d\t", i);
		}
	}
	printf("\n");

	for (int i = 0; i < g_len; i++) {
		printf("  %d  \t", i);
		for (int j = 0; j < g_total_items; j++) {
			printf("%d\t", db[i].item_present[j]);
		}
		printf("\n");
	}

	printf("Validating distribution of values: %d / %d\n", cnt_1, g_total_items * g_len);
}

/*
int main() {
    db = (entry*) malloc(len * sizeof(entry));
    generate_dataset();
    print_dataset();
    return 0;
}
*/
