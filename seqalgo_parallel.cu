#include "dataset.h"
#include "seqalgo.h"

using namespace std;

string output_file = "frequent_itemsets.txt";
string output_file_naive = "frequent_itemsets_naive.txt";

__global__ void cuda_compute_li(int *dev_inp_li, int *dev_inp_txids, int *dev_out_li, int *dev_out_txids, int len_eq_class, int total_items, int total_tx, int Th, int level)
{
    int id = blockDim.x * blockIdx.x + threadIdx.x; // tid
	//get the pairs to work on from tid
	//do the interesection of txids and see if the count is > Th
	// if Yes, add the next li and next txids in the output array
	//txid1 is dev_inp_txids[id*NUM_TX]
	int index1 = 0;
	int index2 = 0;
	int cnt = 0;
	int flag = 0;
	for(int i = 0; i < len_eq_class - 1; i++){
		for(int j = i+1; j <= len_eq_class -1; j++){
			if(cnt == id){
				index1 = i;
				index2 = j;
				flag = 1;
				break;
			}
			cnt++;
		}
		if(flag == 1){
			break;
		}
	}

	//printf("For tid: %d, the index1 = %d, index2 = %d \n", id, index1, index2);

	if(index1 == index2){
		//printf("LEAVING: index1 = %d, index2 = %d\n",index1, index2);
		return;
	}

	
	//printf("My candidates are: (%d %d) and (%d %d) \n", dev_inp_li[index1*(level-1)], dev_inp_li[index1*(level-1) + 1], dev_inp_li[index2 * (level - 1)], dev_inp_li[index2 * (level - 1) +1]);
	/*
	printf("txid1: ");
	for(int i = 0; i < NUM_TX; i++){
		if(dev_inp_txids[index1*NUM_TX + i] != -1){
			printf("%d \t", dev_inp_txids[index1*NUM_TX + i]);
		}
		else{
			break;
		}
	}
	printf("\n txid2: ");
	for(int i = 0; i < NUM_TX; i++){
		if(dev_inp_txids[index2*NUM_TX + i] != -1){
			printf("%d \t", dev_inp_txids[index2*NUM_TX + i]);
		}
		else{
			break;
		}
	}
	*/
	//Step: intersection of txids
	int i_txid = index1*total_tx;
	int k_txid = index2*total_tx;
	cnt = 0;
	while( (i_txid < (index1 + 1)*total_tx) && (k_txid < (index2 + 1)*total_tx) )
	{
		if(dev_inp_txids[i_txid] == -1 || dev_inp_txids[k_txid] == -1){
            break;
        }
		else if(dev_inp_txids[i_txid] == dev_inp_txids[k_txid]){
			//store common element
			dev_out_txids[id*total_tx + cnt] = dev_inp_txids[i_txid];
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

	if(cnt >= Th){
		//printf("Cnt is :%d, Valid candidates: ", cnt);
		//printf("(%d %d) and (%d %d) \n", dev_inp_li[index1*(level-1)], dev_inp_li[index1*(level-1) + 1], dev_inp_li[index2 * (level - 1)], dev_inp_li[index2 * (level - 1) +1]);
		//Step: UNION
		int gap = level - 1;
		int i_li = index1*gap; // 1st element index in li
		int k_li = index2*gap; // 2nd element index in li
		cnt = 0;
		int anchor_pt_li = id*level;
		while((i_li < (index1+1)*gap) && (k_li < (index2+1)*gap)){ 
			if(dev_inp_li[i_li] == dev_inp_li[k_li]){
				//store common element
				//lv_li_arr[cnt] = dev_inp_li[i_li];
				dev_out_li[anchor_pt_li + cnt] = dev_inp_li[i_li];
				//printf("%d \t", dev_inp_li[i_li]);
				cnt++;
				i_li++;
				k_li++;
			}

			else {
				if(dev_inp_li[i_li] < dev_inp_li[k_li]){
					//lv_li_arr[cnt] = dev_inp_li[i_li];
					dev_out_li[anchor_pt_li + cnt] = dev_inp_li[i_li];
					//printf("%d \t", dev_inp_li[i_li]);
					cnt++;
					i_li++;
				}
				else{
					//lv_li_arr[cnt] = dev_inp_li[k_li];
					dev_out_li[anchor_pt_li + cnt] = dev_inp_li[k_li];
					//printf("%d \t", dev_inp_li[k_li]);
					cnt++;
					k_li++;
				}
			}
		}

		if(i_li < (index1+1)*gap){
			while(i_li < (index1+1)*gap){
				dev_out_li[anchor_pt_li + cnt] = dev_inp_li[i_li];
				//printf("%d \t", dev_inp_li[i_li]);
				cnt++;
				i_li++;

			}
		}
		else{
			while(k_li < (index2+1)*gap){
				dev_out_li[anchor_pt_li + cnt] = dev_inp_li[k_li];
				//printf("%d \t", dev_inp_li[k_li]);
				cnt++;
				k_li++;

			}
		}

	}

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
	indexes.push_back(li.size());

	vector<vector<int>> li_next;
	vector<set<int>> litxids_next;
	
	/* printf("Index array for level %d is: \t", level);
	for (int i = 0; i < indexes.size() - 1; i++) {
		printf("%d\t", indexes[i]);
	} */

	/* for (int i = 0; i < indexes.size() - 1; i++) {
		printf("Eq class %d has elements: %d\n", i, indexes[i+1] - indexes[i]);
	} */

	// Send this to the GPU: 
	// 1. Partial index array so that each thread can work on it's candidate
	// 2. Send corresponding li and txids. 
	// 3. Output would be a linear array containing the Lis and Txids for next level

	/*printf("This is how the txids look: \n");
	for(int i = 0; i < litxids.size(); i++){
		set<int>::iterator it;
		int cnt = 0;
		for (it = litxids[i].begin(); it != litxids[i].end(); it++) {
			printf("%d \t", *it);
		}
		printf("\n");
	}*/

	for (int i = 0; i < indexes.size() - 1; i++) {
		int len_eq_class = indexes[i+1] - indexes[i];
		int max_candidates = ((len_eq_class * (len_eq_class - 1)) / 2);
		if(max_candidates > 0){
			int byteLen_eq_class = sizeof(int) * len_eq_class;
			int* eq_class_li; // linear array while li is 2D
			int* eq_class_txids; // linear array while litxids is 2D
			cudaMallocManaged(&eq_class_li, byteLen_eq_class * (level - 1));
			cudaMallocManaged(&eq_class_txids, byteLen_eq_class * (NUM_TX));
			//printf("Level %d, EQ class : %d, Length of eq class is: %d\n", level, i, len_eq_class);
			//preparing the data to send to GPU
			for(int j = 0; j < len_eq_class; j++){
				int anchor_pt_li = j * (level - 1);
				int anchor_pt_txid = j * (NUM_TX);
				for(int x = 0; x < level; x++){
					eq_class_li[anchor_pt_li + x] = li[indexes[i] + j][x];
				}
				
				for(int x = 0; x < NUM_TX; x++){
					eq_class_txids[anchor_pt_txid + x] = -1;
				}

				set<int>::iterator it;
				int cnt = 0;
				for (it = litxids[indexes[i] + j].begin(); it != litxids[indexes[i] + j].end(); it++) {
					eq_class_txids[anchor_pt_txid + cnt] = *it;
					cnt++;
				}
			}
			int * recv_li;
			int * recv_txids;
			int byteLen_li_out = max_candidates * sizeof(int) * level;
			int byteLen_txid_out = max_candidates * sizeof(int) * NUM_TX;
			cudaMallocManaged(&recv_li, byteLen_li_out);
			cudaMallocManaged(&recv_txids, byteLen_txid_out);

			for(int j = 0; j < max_candidates; j++){
				for(int x = 0; x < level; x++){
					recv_li[j*level + x] = 0;
				}
				for(int x = 0; x < NUM_TX; x++){
					recv_txids[j*NUM_TX + x] = -1;
				}
			}

			//launch the kernel
			cuda_compute_li<<<max_candidates,1>>>(eq_class_li, eq_class_txids, recv_li,recv_txids, len_eq_class, NUM_ITEMS,NUM_TX, THRESHOLD,level);
			cudaDeviceSynchronize();

			//put the GPU computed data into li_next and litxids_next
			//printf("max_candidates are: %d, Got this from the GPU: \n", max_candidates);
			/* for(int j = 0; j < max_candidates; j++){
				for(int x = 0; x < level; x++){
						printf("%d",recv_li[j*level + x] );
					}
				printf("\n");
			}

			for(int j = 0; j < max_candidates; j++){
				for(int x = 0; x < NUM_TX; x++){
						printf("%d",recv_txids[j*NUM_TX + x] );
					}
				printf("\n");
			} */

			for(int j = 0; j < max_candidates; j++){
				if(recv_li[j*level] != 0){
					vector<int> items;
					for(int x = 0; x < level; x++){
						//printf("%d",recv_li[j*level + x] );
						 items.push_back(recv_li[j*level + x]);
					}
					li_next.push_back(items);
					set<int> txids;
					//printf("TXIDS are: \n");
					for(int x = 0; x < NUM_TX; x++){
						if(recv_txids[j*NUM_TX + x] != -1){
							//printf("%d\t",recv_txids[j*NUM_TX + x] );
							txids.insert(recv_txids[j*NUM_TX + x]);
						}
						else{
							break;
						}
					}
					litxids_next.push_back(txids);
				}
			}
			cudaFree(eq_class_li);
			cudaFree(eq_class_txids);
			cudaFree(recv_li);
			cudaFree(recv_txids);


		}
	}
	//printf("Length of L%d is: %d\n", level, li_next.size());

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

	/*
	for (int i = 0; i < indexes.size() - 1; i++) {
		printf("Level: %d, Eq class %d has elements: %d\n", level, i, indexes[i+1] - indexes[i]);
	}
	*/

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

#if DEBUG
	print_dataset(db);
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
