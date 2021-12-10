#include "dataset.h"
int len = 10;
int total_items = 10;
struct entry *db;
int cnt_1 = 0;

struct entry{
    int txid;
    int item_present[100]; //100 is upper bound
};

int get_probabilistic_value(){
    int val = 0;
    int dist_len = 10000;
    int rand_val = rand()%dist_len;
    //printf("Rand value is : %d\n", rand_val);
    float chance = 0.5; // 80% chance to get 0
    if(rand_val > chance * dist_len){ 
        val = 1;
        cnt_1++;
    }
    return val;
}

void generate_dataset(){
    //this fills up the database
    srand (time(NULL));
    for(int i = 0; i < len; i++){
        db[i].txid = i;
        for(int j = 0; j < total_items;j++){
            db[i].item_present[j] = get_probabilistic_value(); // TODO - some probabilistic function should give a value
        }
    }
}

void print_dataset(){
    //first row
    for(int i = 0; i < total_items + 1; i++){
        if(i == 0){
            printf("TX ID\t");
        }
        else{
            printf("Item %d\t", i);
        }
    }
    printf("\n");

    for(int i = 0; i < len; i++){
        printf("  %d  \t", i);
        for(int j = 0; j < total_items;j++){
            printf("%d\t", db[i].item_present[j]);
        }
        printf("\n");
    }

    printf("Validating distribution of values: %d / %d\n", cnt_1, total_items * len);
}


int main(){
    db = (struct entry*)malloc(len*sizeof(struct entry));
    generate_dataset();
    print_dataset();
    return 0;
}
