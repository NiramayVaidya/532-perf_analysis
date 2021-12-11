#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <iostream>

typedef struct entry {
    int txid;
    int item_present[100]; // 100 is upper bound
} entry;

void generate_dataset(entry *db, int len, int total_items);
void print_dataset(entry *db);
