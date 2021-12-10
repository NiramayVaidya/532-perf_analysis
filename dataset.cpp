#include "dataset.h"
int len = 10;
int total_items = 10;
struct entry *db;

struct entry{
    int txid;
    int item_present[100]; //100 is upper bound
};

void generate_dataset(){
    //this fills up the database
    for(int i = 0; i < len; i++){
        db[i].txid = i;
        for(int j = 0; j < total_items;j++){
            db[i].item_present[j] = 1; // TODO - some probabilistic function should give a value
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
}


int main(){
    db = (struct entry*)malloc(len*sizeof(struct entry));
    generate_dataset();
    print_dataset();
    return 0;
}
