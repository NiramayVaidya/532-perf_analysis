//#include <iostream.h>
#include "dataset.h"

#define NUM_TX 10
#define NUM_ITEMS 8
#define THRESHOLD NUM_TX*0.5
#define LEVEL 50
entry *db;
int l1_items[NUM_ITEMS]; 
int l1_tx_record[NUM_ITEMS][NUM_TX]; // custom transpose matrix

void init(){
    db = (entry*) malloc(NUM_TX * sizeof(entry));
    generate_dataset(db, NUM_TX, NUM_ITEMS);
    for(int i = 0; i < NUM_ITEMS;i++){
        l1_items[i] = 0;
    }

    //fill this with -1 to indicate invalid cell
    for(int i = 0; i < NUM_ITEMS;i++){
        for(int j = 0; j < NUM_TX; j++){
            l1_tx_record[i][j] = -1;
        }
    }

}

void generate_l1(){
    //STEPS: 
    //1. Go through each entry for each item and see if that item is present. 
    //2. If the count is > Th, store the entries where it was present. 
    for(int i = 0; i < NUM_ITEMS; i++){
        int cnt = 0;
        for(int j = 0; j < NUM_TX; j++){
            if(db[j].item_present[i] == 1){
                l1_tx_record[i][cnt] = j; // store the TXid's
                cnt++;
            }
        }
        if(cnt >= THRESHOLD){
            l1_items[i] = 1;
        }
    }
}

void print_l1(){
    for(int i = 0; i < NUM_ITEMS; i++){
        printf("l1[%d] = %d\t", i, l1_items[i]);
    }
    printf("\n");
    //print the transpose matrix
    for(int i = 0; i < NUM_ITEMS;i++){
        if(l1_items[i] == 1){ // TODO - optimize by storing in linear form like l1_tx_record to minimize all checks
            printf("Item %d - ", i);
            for(int j = 0; j < NUM_TX; j++){
                if(l1_tx_record[i][j] != -1){
                    printf("%d\t", l1_tx_record[i][j]);
                }
                else{
                    break;
                }
            }
            printf("\n");
    }
}
}

void generate_l2{
    int lv_valid_l1[NUM_ITEMS];
    int iter = 0;
    
    //mark all entries as -1
    for(int i = 0; i < NUM_ITEMS; i++){
        lv_valid_l1[i] = -1;
    }
    
    //setting local array
    for(int i = 0; i < NUM_ITEMS; i++){
        if(l1_items[i] == 1){
            lv_valid_l1[iter] = i;
            iter++;
        }
    }

    while() 
    


}

int main(){
    
    init();
    print_dataset(db);
    
    //generate L1 - count if individual item occurs more than THRESHOLD in DB
    generate_l1();
    print_l1();

    //generate L2 - combinations of two from L1 and occurence > THRESHOLD
    generate_l2();
    print_l2();
    
    
    return 0;
}
