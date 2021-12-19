#include "dataset.h"
#include "seqalgo.h"
#include <stdio.h>
//#include <cuda_runtime_api.h>
//#include <cuda.h>

using namespace std;

string output_file = "frequent_itemsets.txt";
string output_file_naive = "frequent_itemsets_naive.txt";

//int *dev_inp_li, *dev_inp_txids, *dev_inp_indices, *dev_out_li, *dev_out_txids;
// CUDA Kernel
// Each eq class decided by indices array is parallelized. 
__global__ void cuda_compute_li(int *dev_inp_li, int *dev_inp_txids, int *dev_inp_indices, int *dev_out_li, int *dev_out_txids, int index_len, int total_items, int total_tx, int Th, int level)
{
    int id = blockDim.x * blockIdx.x + threadIdx.x;
    printf("-------------------------\n\n");
    printf("INSIDE CUDA: MY TID IS: %d\n", id);
    if(id < index_len - 1){
        //id becomes the index position
        //Step1: find the pairs of next Li
        //Step2: compare with threshold - implement intersection
        //Step3: create the corresponding txids
        //eg: [0,3,5,6] - pairs would be 01, 02, 12
        
        printf("indices array: >>>>>>>>>>>> \n");
        for(int x = 0; x < index_len; x++){
            printf("%d \t", dev_inp_indices[x]);
        }
        printf("End of indices array check\n");
        int start_index = dev_inp_indices[id];
        int end_index = dev_inp_indices[id+1];
        //finding pairs
        for(int i = start_index; i < end_index - 1; i++){
            //within this eq class, find the intersection of txids and count the occurence
            for(int k = i + 1; k <= end_index - 1; k++){
                //check for i,k pair
                int i_txid = i*total_tx;
                int k_txid = k*total_tx;
                int cnt = 0;
                int lv_arr[NUM_TX];
                for(int i = 0; i < total_tx; i++){
                    lv_arr[i] = -1;
                }

                //TODO print the corresponding data: 
                //printf(                      );
                for(int x = 0; x < level-1; x++){
                    printf("");
                }
                
                //int lv_it_k = k_txid;
                /*
                printf("First element's TXIDS\n");
                for(int x = 0; x < total_tx; x++){
                    printf("%d\t", dev_inp_txids[i_txid + x]);
                }

                
                printf("\n Second element's TXIDS\n");
                for(int x = 0; x < total_tx; x++){
                    printf("%d\t", dev_inp_txids[k_txid + x]);
                }
                printf("\n");
                */
                //intersection
                while((i_txid < (i+1)*total_tx) && (k_txid < (k+1)*total_tx)){
                    if(dev_inp_txids[i_txid] == -1 || dev_inp_txids[k_txid] == -1){
                        break;
                    }

                    else if(dev_inp_txids[i_txid] == dev_inp_txids[k_txid]){
                        //store common element
                        lv_arr[cnt] = dev_inp_txids[i_txid];
                        cnt++;
                        i_txid++;
                        k_txid++;
                    }

                    else if (dev_inp_txids[i_txid] > dev_inp_txids[k_txid]){
                        k_txid++;
                    }
                    else{
                        i_txid++;
                    }

                }
                
                //check if count > Th
                if(cnt >= Th){
                    //do union and copy the intersection wala txid
                    int gap = level - 1;
                    int i_li = i*gap; // 1st element index in li
                    int k_li = k*gap; // 2nd element index in li
                    //int lv_li_arr[level];
                    printf("Value of cnt is: %d for pair: (%d %d), (%d %d)\n", cnt,dev_inp_li[i_li], dev_inp_li[i_li + 1], dev_inp_li[k_li], dev_inp_li[k_li+ 1] );

                    int anchor_pt_li = (i+k)*level;
                    cnt = 0;
                    printf("INSIDE CUDA: printing the next li: \n");
                    //union
                    while((i_li < (i+1)*gap) && (k_li < (k+1)*gap)){
                        
                        if(dev_inp_li[i_li] == dev_inp_li[k_li]){
                            //store common element
                            //lv_li_arr[cnt] = dev_inp_li[i_li];
                            dev_out_li[anchor_pt_li + cnt] = dev_inp_li[i_li];
                            printf("%d \t", dev_inp_li[i_li]);
                            cnt++;
                            i_li++;
                            k_li++;
                        }

                        else {
                            if(dev_inp_li[i_li] < dev_inp_li[k_li]){
                                //lv_li_arr[cnt] = dev_inp_li[i_li];
                                dev_out_li[anchor_pt_li + cnt] = dev_inp_li[i_li];
                                printf("%d \t", dev_inp_li[i_li]);
                                cnt++;
                                i_li++;
                            }
                            else{
                                //lv_li_arr[cnt] = dev_inp_li[k_li];
                                dev_out_li[anchor_pt_li + cnt] = dev_inp_li[k_li];
                                printf("%d \t", dev_inp_li[k_li]);
                                cnt++;
                                k_li++;
                            }
                        }
                    }
                    if(i_li < (i+1)*gap){
                        while(i_li < (i+1)*gap){
                            dev_out_li[anchor_pt_li + cnt] = dev_inp_li[i_li];
                            printf("%d \t", dev_inp_li[i_li]);
                            cnt++;
                            i_li++;

                        }
                    }
                    else{
                        while(k_li < (k+1)*gap){
                            dev_out_li[anchor_pt_li + cnt] = dev_inp_li[k_li];
                            printf("%d \t", dev_inp_li[k_li]);
                            cnt++;
                            k_li++;

                        }
                    }
                    printf("\n");
                    
                    // store the txids and union in the output array
                    /*
                    for(int z = 0; z < level; z++){
                        dev_out_li[anchor_pt_li + z] = lv_li_arr[z];
                    }*/

                    int anchor_pt_txid = (i+k)*total_tx;
                    printf("INSIDE CUDA: printing the next txids: \n");
                    for(int z = 0; z < total_tx; z++){
                        dev_out_txids[anchor_pt_txid + z] = lv_arr[z];
                        printf("%d \t", lv_arr[z]);
                    }
                    printf("\n");

            }
        }
    }
}

    printf("-------------------------\n\n");
}

tuple<vector<vector<int>>, vector<set<int>>, long long> compute_li(vector<vector<int>> li, vector<set<int>> litxids, int offset, int level) {
	vector<int> indexes;
	long long time = 0;
	
	auto start = std::chrono::high_resolution_clock::now();

	indexes.push_back(0);
	for (int i = 0; i < li.size() - 1; i++) {
		bool all_same = true;
		for (int j = 0; j <= offset; j++) {
			if (li[i][j] != li[i + 1][j]) {
				all_same = false;
				break;
			}
		}
		if (!all_same) {
			indexes.push_back(i + 1);
		}
	}

    //print indices
    
    indexes.push_back(li.size());
    vector<int>::iterator ptr;
    printf("Indices for level: %d\n ", level);
    for (ptr = indexes.begin(); ptr < indexes.end(); ptr++){
        cout << *ptr << " ";
    }
	printf("\n");
    

    //Inputs here are: 
    //indexes array: [0,2,4]
    //L2: [{1, 5}	{1, 6}	{3, 5}	{3, 6}	{5, 6}]
    //txids:[[], [], [], [], []] - the number of elements in L2 and txids should be same, like index of AB is L2 should also point to its corresponding txids. 
    //To do work with GPU: 
    //INPUT to GPU: index array, L2, txids
    //OUTPUT from GPU: L3, txids
    //convert L2 vector to linear array - each itemset element separated by the level (3 means 2 len itemset)
    //convert txid set to linear array- each entry separated by len of dataset.
    //have the corresponding output arrays. 
	
    //convert vector to array - linearizing the vector of vector
    int byteLen_li = sizeof(int) * li.size() * (level-1); // total integers stored in the Itemset vector
    int * lv_arr = (int *)malloc(byteLen_li);

    for(int i = 0; i < li.size(); i++){
        for(int j = 0; j < level - 1; j++){
            int anchor_pt = i*(level-1);
            lv_arr[anchor_pt + j] = li[i][j];
            //printf("%d \t ", li[i][j]);
        }
       // printf("\n");
    }

    int byteLen_indices = sizeof(int) * indexes.size();
    int * lv_index_arr = (int*)malloc(byteLen_indices);
    for(int i = 0; i < indexes.size(); i++){
            lv_index_arr[i] = indexes[i];
    }



    //convert set to array - linearizing the vector of set
    int byteLen_txid = sizeof(int) * litxids.size() * (NUM_TX); // total integers stored in set (max)
    int * lv_arr2 = (int *)malloc(byteLen_txid);

    for(int i = 0; i < litxids.size(); i++){
        int anchor_pt = i*(NUM_TX);
        for(int j = 0; j < NUM_TX; j++){
            lv_arr2[anchor_pt + j] = -1;
        }
        
        set<int>::iterator it;
        int cnt = 0;
		for (it = litxids[i].begin(); it != litxids[i].end(); it++) {
            lv_arr2[anchor_pt + cnt] = *it;
            cnt++;
		}

    }
   /* 
    printf("The corresponding tTXIDS are: \n");

    for(int i = 0; i < litxids.size(); i++){
        for(int j = 0; j < NUM_TX; j++){
            int anchor_pt = i*(NUM_TX);
            printf("%d \t", lv_arr2[anchor_pt + j]);
        }
        printf("\n");
        
    }

    printf("Total elements in Li are: %d\n", li.size());
    printf("Total elements in Txid are: %d\n", litxids.size());
    
*/

    //create the input and output arrays and do cuda malloc
    int *dev_inp_li, *dev_inp_txids, *dev_inp_indices, *dev_out_li, *dev_out_txids;
    cudaMalloc((void**)&dev_inp_li, byteLen_li);
    cudaMalloc((void**)&dev_inp_txids, byteLen_txid);
    cudaMalloc((void**)&dev_inp_indices, byteLen_indices);
    int byteLen_li_out = ((li.size() * (li.size() - 1)) / 2) * sizeof(int) * level;
    cudaMalloc((void**)&dev_out_li, byteLen_li_out); // len is decided by max combinations which comes from len of li = (len_li) * (len_li-1) * 0.5
    int byteLen_txid_out = ((li.size() * (li.size() - 1)) / 2) * sizeof(int) * NUM_TX;
    cudaMalloc((void**)&dev_out_txids, byteLen_txid_out);
    
    int * recv_li = (int *)malloc(byteLen_li_out);
    int * recv_txids = (int *)malloc(byteLen_txid_out);
    cudaMemcpy(dev_inp_li,lv_arr , byteLen_li, cudaMemcpyHostToDevice);
    cudaMemcpy(dev_inp_txids, lv_arr2, byteLen_txid, cudaMemcpyHostToDevice);
    cudaMemcpy(dev_inp_indices, lv_index_arr, byteLen_indices, cudaMemcpyHostToDevice);

    int len_indices = indexes.size();
    printf("Launching kernel!\n");
    //TODO change total TBs to #eq_classes
    cuda_compute_li<<<1,1>>>(dev_inp_li, dev_inp_txids, dev_inp_indices, dev_out_li,dev_out_txids, len_indices, NUM_ITEMS,NUM_TX, THRESHOLD,level);
    cudaMemcpy(recv_li, dev_out_li, byteLen_li_out, cudaMemcpyDeviceToHost);
    cudaMemcpy(recv_txids, dev_out_txids,byteLen_txid_out, cudaMemcpyDeviceToHost);
    


    //print recevied li
    printf("##### GPU computed Lis are: #####\n");
    for(int i = 0; i < byteLen_li_out/sizeof(int); i = i + level){
        for(int j = 0; j < level; j++){
            int anchor_pt = i*(level);
            //lv_arr[anchor_pt + j] = li[i][j];
            printf("%d \t ", recv_li[anchor_pt + j]);
        }
        printf("\n");
    }
    
    printf("##### GPU computed TXIDs are: #####\n");
    for(int i = 0; i < byteLen_txid_out/sizeof(int); i = i + NUM_TX){
        for(int j = 0; j < NUM_TX; j++){
            int anchor_pt = i*(NUM_TX);
            //lv_arr[anchor_pt + j] = li[i][j];
            printf("%d \t ", recv_txids[anchor_pt + j]);
        }
        printf("\n");
    }

    vector<vector<int>> li_next;
	vector<set<int>> litxids_next;
	/*
    for (int i = 0; i < indexes.size() - 1; i++) {
		for (int j = indexes[i]; j < indexes[i + 1] - 1; j++) {
			for (int k = j + 1; k <= indexes[i + 1] - 1; k++) {
				set<int> txids;
				set_intersection(litxids[j].begin(), litxids[j].end(), litxids[k].begin(), litxids[k].end(), inserter(txids, txids.begin()));
				if (txids.size() >= THRESHOLD) {
					vector<int> items;
					set_union(li[j].begin(), li[j].end(), li[k].begin(), li[k].end(), back_inserter(items));
					li_next.push_back(items);
					litxids_next.push_back(txids);
				}
			}
		}
	}*/
	
	auto elapsed = std::chrono::high_resolution_clock::now() - start;
	time += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

#if INFO
		cout << "l" << level << " ->" << endl;
		for (int i = 0; i < li_next.size(); i++) {
			for (int j = 0; j < li_next[i].size(); j++) {
				cout << li_next[i][j];
			}
			cout << "\t";
		}
		cout << endl;
#endif

#if DEBUG
		cout << "l" << level << " txids ->" << endl;
		for (int i = 0; i < litxids_next.size(); i++) {
			for (int j = 0; j < li_next[i].size(); j++) {
				cout << li_next[i][j];
			}
			cout << " = ";
			set<int>::iterator it;
			for (it = litxids_next[i].begin(); it != litxids_next[i].end(); it++) {
				cout << *it << " ";
			}
			cout << endl;
		}
#endif

	tuple<vector<vector<int>>, vector<set<int>>, long long> ret;
	ret = make_tuple(li_next, litxids_next, time);
	return ret;
}

#if NAIVE_METHOD
tuple<vector<vector<int>>, long long> compute_li_naive(vector<vector<int>> li, int level, int offset, entry *db) {
	vector<int> indexes;
	long long time = 0;
	
	auto start = std::chrono::high_resolution_clock::now();

	indexes.push_back(0);
	for (int i = 0; i < li.size() - 1; i++) {
		bool all_same = true;
		for (int j = 0; j <= offset; j++) {
			if (li[i][j] != li[i + 1][j]) {
				all_same = false;
				break;
			}
		}
		if (!all_same) {
			indexes.push_back(i + 1);
		}
	}
	indexes.push_back(li.size());

	vector<vector<int>> li_next;
	for (int i = 0; i < indexes.size() - 1; i++) {
		for (int j = indexes[i]; j < indexes[i + 1] - 1; j++) {
			for (int k = j + 1; k <= indexes[i + 1] - 1; k++) {
				vector<int> items;
				set_union(li[j].begin(), li[j].end(), li[k].begin(), li[k].end(), back_inserter(items));
				int count = 0;
				for (int l = 0; l < NUM_TX; l++) {
					bool all_present = true;
					for (int m = 0; m < items.size(); m++) {
						if (db[l].item_present[items[m] - 1] != 1) {
							all_present = false;
							break;
						}
					}
					if (all_present) {
						count++;
					}
				}
				if (count >= THRESHOLD) {
					li_next.push_back(items);
				}
			}
		}
	}
	
	auto elapsed = std::chrono::high_resolution_clock::now() - start;
	time += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

#if INFO
		cout << "l" << level << " ->" << endl;
		for (int i = 0; i < li_next.size(); i++) {
			for (int j = 0; j < li_next[i].size(); j++) {
				cout << li_next[i][j];
			}
			cout << "\t";
		}
		cout << endl;
#endif

	tuple<vector<vector<int>>, long long> ret;
	ret = make_tuple(li_next, time);
	return ret;
}
#endif

int main() {
	entry *db = (entry *) malloc(NUM_TX * sizeof(entry));

	generate_dataset(db, NUM_TX, NUM_ITEMS);

	print_dataset(db);
#if DEBUG
#endif

	vector<string> all_freq_itemsets;
	long long total_time = 0;
#if NAIVE_METHOD
	vector<string> all_freq_itemsets_naive;
	long long total_time_naive = 0;
#endif

	vector<int> l1;

	auto start = std::chrono::high_resolution_clock::now();

	/* Computing l1 by checking if occurences of each item across all
	 * transactions exceed the threshold
	 */
	for (int i = 0; i < NUM_ITEMS; i++) {
		int count = 0;
		for (int j = 0; j < NUM_TX; j++) {
			if (db[j].item_present[i] == 1) {
				count++;
			}
		}
		if (count >= THRESHOLD) {
			l1.push_back(i + 1);
		}
	}

	auto elapsed = std::chrono::high_resolution_clock::now() - start;
	total_time += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
#if NAIVE_METHOD
	total_time_naive += total_time;
#endif

	for (int i = 0; i < l1.size(); i++) {
		all_freq_itemsets.push_back("{" + to_string(l1[i]) + "}");
#if NAIVE_METHOD
		all_freq_itemsets_naive.push_back("{" + to_string(l1[i]) + "}");
#endif
	}

#if INFO
	cout << "l1 ->" << endl;
	for (int i = 0; i < l1.size(); i++) {
		cout << l1[i] << "\t";
	}
	cout << endl;
#endif

	vector<vector<int>> l2;
	vector<set<int>> newdb;
	
	start = std::chrono::high_resolution_clock::now();

	/* Computing l2 by combining pairs within l1
	 * Restructuring the dataset to the format -> itemset : txids
	 */
	for (int i = 0; i < l1.size(); i++) {
		for (int j = i + 1; j < l1.size(); j++) {
			int count = 0;
			set<int> txids;
			for (int k = 0; k < NUM_TX; k++) {
				if (db[k].item_present[l1[i] - 1] == 1 && db[k].item_present[l1[j] - 1] == 1) {
					txids.insert(k);
					count++;
				}
			}
			if (count >= THRESHOLD) {
				vector<int> items;
				items.push_back(l1[i]);
				items.push_back(l1[j]);
				l2.push_back(items);
				newdb.push_back(txids);
			}
		}
	}
	
	elapsed = std::chrono::high_resolution_clock::now() - start;
	total_time += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

	for (int i = 0; i < l2.size(); i++) {
		stringstream items_stream;
		copy(l2[i].begin(), l2[i].end(), ostream_iterator<int>(items_stream, ", "));
		all_freq_itemsets.push_back("{" + items_stream.str().substr(0, items_stream.str().size() - 2) + "}");
	}

#if INFO
	cout << "l2 ->" << endl;
	for (int i = 0; i < l2.size(); i++) {
		cout << l2[i][0] << l2[i][1] << "\t";
	}
	cout << endl;
#endif

#if DEBUG
	cout << "Reformed dataset ->" << endl;
	for (int i = 0; i < newdb.size(); i++) {
		cout << l2[i][0] << l2[i][1] << " = ";
		set<int>::iterator it;
		for (it = newdb[i].begin(); it != newdb[i].end(); it++) {
			cout << *it << " ";
		}
		cout << endl;
	}
#endif

	/* Computing li after l2 for all i up to the permitted level
	 */
	vector<vector<int>> li = l2;
	vector<set<int>> litxids = newdb;
	int offset = 0;
	for (int i = 3; i <= LEVEL; i++) {
		if (li.size() >= 1) {
			tuple<vector<vector<int>>, vector<set<int>>, long long> ret = compute_li(li, litxids, offset, i);
			li = get<0>(ret);
			litxids = get<1>(ret);
			total_time += get<2>(ret);
			for (int j = 0; j < li.size(); j++) {
				stringstream items_stream;
				copy(li[j].begin(), li[j].end(), ostream_iterator<int>(items_stream, ", "));
				all_freq_itemsets.push_back("{" + items_stream.str().substr(0, items_stream.str().size() - 2) + "}");
			}
			offset++;
		}
		else {
			break;
		}
	}

	fstream out_file;
	out_file.open(output_file, fstream::out | fstream::trunc);
	int i;
#if INFO
	cout << "Frequent itemsets ->" << endl;
#endif
	for (i = 0; i < all_freq_itemsets.size() - 1; i++) {
#if INFO
		cout << all_freq_itemsets[i] << "\t";
#endif
		out_file << all_freq_itemsets[i] << endl;
	}
#if INFO
	cout << all_freq_itemsets[i] << endl;
#endif
	out_file << all_freq_itemsets[i] << endl;
	out_file.close();

	cout << "Total execution time (optimal algorithm) = " << total_time << " us" << endl;

#if NAIVE_METHOD
	vector<vector<int>> l2_naive;
	
	start = std::chrono::high_resolution_clock::now();

	/* Computing l2 by combining pairs within l1
	 * No restructuring of the original dataset required in the naive case
	 */
	for (int i = 0; i < l1.size(); i++) {
		for (int j = i + 1; j < l1.size(); j++) {
			int count = 0;
			for (int k = 0; k < NUM_TX; k++) {
				if (db[k].item_present[l1[i] - 1] == 1 && db[k].item_present[l1[j] - 1] == 1) {
					count++;
				}
			}
			if (count >= THRESHOLD) {
				vector<int> items;
				items.push_back(l1[i]);
				items.push_back(l1[j]);
				l2_naive.push_back(items);
			}
		}
	}
	
	elapsed = std::chrono::high_resolution_clock::now() - start;
	total_time_naive += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

	for (int i = 0; i < l2_naive.size(); i++) {
		stringstream items_stream;
		copy(l2_naive[i].begin(), l2_naive[i].end(), ostream_iterator<int>(items_stream, ", "));
		all_freq_itemsets_naive.push_back("{" + items_stream.str().substr(0, items_stream.str().size() - 2) + "}");
	}

#if INFO
	cout << "l2 ->" << endl;
	for (int i = 0; i < l2_naive.size(); i++) {
		cout << l2_naive[i][0] << l2_naive[i][1] << "\t";
	}
	cout << endl;
#endif

	/* Computing li after l2 for all i up to the permitted level
	 * Always use the original dataset
	 */
	vector<vector<int>> li_naive = l2_naive;
	offset = 0;
	for (int i = 3; i <= LEVEL; i++) {
		if (li_naive.size() >= 1) {
			tuple<vector<vector<int>>, long long> ret = compute_li_naive(li_naive, i, offset, db);
			li_naive = get<0>(ret);
			total_time_naive += get<1>(ret);
			for (int j = 0; j < li_naive.size(); j++) {
				stringstream items_stream;
				copy(li_naive[j].begin(), li_naive[j].end(), ostream_iterator<int>(items_stream, ", "));
				all_freq_itemsets_naive.push_back("{" + items_stream.str().substr(0, items_stream.str().size() - 2) + "}");
			}
			offset++;
		}
		else {
			break;
		}
	}
	
	fstream out_file_naive;
	out_file_naive.open(output_file_naive, fstream::out | fstream::trunc);
#if INFO
	cout << "Frequent itemsets ->" << endl;
#endif
	for (i = 0; i < all_freq_itemsets_naive.size() - 1; i++) {
#if INFO
		cout << all_freq_itemsets_naive[i] << "\t";
#endif
		out_file_naive << all_freq_itemsets_naive[i] << endl;
	}
#if INFO
	cout << all_freq_itemsets_naive[i] << endl;
#endif
	out_file_naive << all_freq_itemsets_naive[i] << endl;
	out_file_naive.close();

	cout << "Total execution time (naive algorithm) = " << total_time_naive << " us" << endl;
#endif

	return 0;
}
