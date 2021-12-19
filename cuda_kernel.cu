//convert vector to array - linearizing the vector of vector
int byteLen_li = sizeof(int) * li.size() * (level-1); // total integers stored in the Itemset vector
int * lv_arr = (int *)malloc(byteLen_li);

for(int i = 0; i < li.size(); i++){
    for(int j = 0; j < level - 1; j++){
        int anchor_pt = i*(level-1)
        lv_arr[anchor_pt + j] = li[i][j];
    }
}

//convert set to array - linearizing the vector of set
int byteLen_txid = sizeof(int) * litxids.size() * (NUM_ITEMS); // total integers stored in the Itemset vector
int * lv_arr2 = (int *)malloc(byteLen_txid);

for(int i = 0; i < litxids.size(); i++){
    for(int j = 0; j < NUM_ITEMS; j++){
        int anchor_pt = i*(NUM_ITEMS)
        lv_arr[anchor_pt + j] = litxids[i][j];
    }
}


//create the input and output arrays and do cuda malloc
int *dev_inp_li, *dev_inp_txids, *dev_out_li, *dev_out_txids;
cudaMalloc((void**)&dev_inp_li, byteLen_li);
cudaMalloc((void**)&dev_inp_txids, byteLen_txid);
int byteLen_li_out = ((li.size() * (li.size() - 1)) / 2) * sizeof(int);
cudaMalloc((void**)&dev_out_li, byteLen_out); // len is decided by max combinations which comes from len of li = (len_li) * (len_li-1) * 0.5
int byteLen_txid_out = ((li.size() * (li.size() - 1)) / 2) * sizeof(int) * NUM_ITEMS;
cudaMalloc((void**)&dev_out_txids, byteLen_out);
