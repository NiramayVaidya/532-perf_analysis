#include "pthreadsalgo.h"

using namespace std;

static entry *db;

static vector<vector<int>> all_freq_itemsets;
long long total_time = 0;

#if NAIVE_METHOD
static vector<vector<int>> all_freq_itemsets_naive;
static long long total_time_naive = 0;
#endif

static vector<vector<int>> l2;
static vector<set<int>> newdb;

static vector<equivalence_class_indexes> ecis;

static string output_file = "frequent_itemsets_pthreads.txt";
static string output_file_naive = "frequent_itemsets_naive_pthreads.txt";

static pthread_mutex_t lock;

static tuple<vector<vector<int>>, vector<set<int>>> compute_li(vector<vector<int>> li, vector<set<int>> litxids, int offset, int level) {
	vector<int> indexes;

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
	}

#if INFO
	cout << "l" << level << " ->" << endl;
	cout << "Count = " << li_next.size() << endl;
	for (int i = 0; i < li_next.size(); i++) {
		int j;
		cout << "{";
		for (j = 0; j < li_next[i].size() - 1; j++) {
			cout << li_next[i][j] << ", ";
		}
		cout << li_next[i][j] << "}\t";
	}
	cout << endl;
#endif

	// cout << "l" << level << " computation done" << endl;

#if DEBUG
	cout << "l" << level << " txids ->" << endl;
	for (int i = 0; i < litxids_next.size(); i++) {
		for (int j = 0; j < li_next[i].size(); j++) {
			cout << li_next[i][j] << " ";
		}
		cout << "= ";
		set<int>::iterator it;
		for (it = litxids_next[i].begin(); it != litxids_next[i].end(); it++) {
			cout << *it << " ";
		}
		cout << endl;
	}
#endif

	tuple<vector<vector<int>>, vector<set<int>>> ret;
	ret = make_tuple(li_next, litxids_next);
	return ret;
}

#if NAIVE_METHOD
static vector<vector<int>> compute_li_naive(vector<vector<int>> li, int offset, int level) {
	vector<int> indexes;

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

#if INFO
	cout << "l" << level << " ->" << endl;
	cout << "Count = " << li_next.size() << endl;
	for (int i = 0; i < li_next.size(); i++) {
		int j;
		cout << "{";
		for (j = 0; j < li_next[i].size() - 1; j++) {
			cout << li_next[i][j] << ", ";
		}
		cout << li_next[i][j] << "}\t";
	}
	cout << endl;
#endif

	// cout << "l" << level << " computation done" << endl;

	return li_next;
}
#endif

static void *launch_compute(void *pos) {
	long i = (long) pos;
	int start = ecis[i].start;
	int end = ecis[i].end;
	int offset = ecis[i].offset;

	// pthread_mutex_lock(&lock);
	if (l2.size() >= 1) {
		// pthread_mutex_unlock(&lock);
		vector<vector<int>> l3;
		vector<set<int>> l3txids;

		for (int j = start; j < end - 1; j++) {
			for (int k = j + 1; k <= end - 1; k++) {
				set<int> txids;
				// pthread_mutex_lock(&lock);
				set_intersection(newdb[j].begin(), newdb[j].end(), newdb[k].begin(), newdb[k].end(), inserter(txids, txids.begin()));
				// pthread_mutex_unlock(&lock);
				if (txids.size() >= THRESHOLD) {
					vector<int> items;
					// pthread_mutex_lock(&lock);
					set_union(l2[j].begin(), l2[j].end(), l2[k].begin(), l2[k].end(), back_inserter(items));
					// pthread_mutex_unlock(&lock);
					l3.push_back(items);
					l3txids.push_back(txids);
				}
			}
		}

#if INFO
		cout << "l3" << " ->" << endl;
		cout << "Count = " << l3.size() << endl;
		for (int i = 0; i < l3.size(); i++) {
			int j;
			cout << "{";
			for (j = 0; j < l3[i].size() - 1; j++) {
				cout << l3[i][j] << ", ";
			}
			cout << l3[i][j] << "}\t";
		}
		cout << endl;
#endif

		// cout << "l3 computation done" << endl;

#if DEBUG
		cout << "l3" << " txids ->" << endl;
		for (int i = 0; i < l3txids.size(); i++) {
			for (int j = 0; j < l3[i].size(); j++) {
				cout << l3[i][j] << " ";
			}
			cout << "= ";
			set<int>::iterator it;
			for (it = l3txids[i].begin(); it != l3txids[i].end(); it++) {
				cout << *it << " ";
			}
			cout << endl;
		}
#endif

		pthread_mutex_lock(&lock);
		all_freq_itemsets.insert(all_freq_itemsets.end(), l3.begin(), l3.end());
		pthread_mutex_unlock(&lock);

		offset++;
		vector<vector<int>> li = l3;
		vector<set<int>> litxids = l3txids;
		for (int i = 4; i <= LEVEL; i++) {
			if (li.size() >= 1) {
				tuple<vector<vector<int>>, vector<set<int>>> ret = compute_li(li, litxids, offset, i);
				li = get<0>(ret);
				litxids = get<1>(ret);
				pthread_mutex_lock(&lock);
				all_freq_itemsets.insert(all_freq_itemsets.end(), li.begin(), li.end());
				pthread_mutex_unlock(&lock);
				offset++;
			}
			else {
				break;
			}
		}
	}
	// pthread_mutex_unlock(&lock);

	return NULL;
}

#if NAIVE_METHOD
static void *launch_compute_naive(void *pos) {
	long i = (long) pos;
	int start = ecis[i].start;
	int end = ecis[i].end;
	int offset = ecis[i].offset;

	// pthread_mutex_lock(&lock);
	if (l2.size() >= 1) {
		// pthread_mutex_unlock(&lock);
		vector<vector<int>> l3;

		for (int j = start; j < end - 1; j++) {
			for (int k = j + 1; k <= end - 1; k++) {
				vector<int> items;
				set_union(l2[j].begin(), l2[j].end(), l2[k].begin(), l2[k].end(), back_inserter(items));
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
					l3.push_back(items);
				}
			}
		}

#if INFO
		cout << "l3" << " ->" << endl;
		cout << "Count = " << l3.size() << endl;
		for (int i = 0; i < l3.size(); i++) {
			int j;
			cout << "{";
			for (j = 0; j < l3[i].size() - 1; j++) {
				cout << l3[i][j] << ", ";
			}
			cout << l3[i][j] << "}\t";
		}
		cout << endl;
#endif

		// cout << "l3 computation done" << endl;

		pthread_mutex_lock(&lock);
		all_freq_itemsets_naive.insert(all_freq_itemsets_naive.end(), l3.begin(), l3.end());
		pthread_mutex_unlock(&lock);

		offset++;
		vector<vector<int>> li = l3;
		for (int i = 4; i <= LEVEL; i++) {
			if (li.size() >= 1) {
				li = compute_li_naive(li, offset, i);
				pthread_mutex_lock(&lock);
				all_freq_itemsets_naive.insert(all_freq_itemsets_naive.end(), li.begin(), li.end());
				pthread_mutex_unlock(&lock);
				offset++;
			}
			else {
				break;
			}
		}
	}

	return NULL;
}
#endif

void pthreadsalgo_run(entry *dataset) {
	// db = (entry *) malloc(NUM_TX * sizeof(entry));
	db = dataset;

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
		vector<int> l1single;
		l1single.push_back(l1[i]);
		all_freq_itemsets.push_back(l1single);
#if NAIVE_METHOD
		all_freq_itemsets_naive.push_back(l1single);
#endif
	}

#if INFO
	cout << "l1 ->" << endl;
	cout << "Count = " << l1.size() << endl;
	for (int i = 0; i < l1.size(); i++) {
		cout << "{" << l1[i] << "}\t";
	}
	cout << endl;
#endif

	// cout << "l1 computation done" << endl;

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

	all_freq_itemsets.insert(all_freq_itemsets.end(), l2.begin(), l2.end());

#if INFO
	cout << "l2 ->" << endl;
	cout << "Count = " << l2.size() << endl;
	for (int i = 0; i < l2.size(); i++) {
		cout << "{" << l2[i][0] << ", " << l2[i][1] << "}\t";
	}
	cout << endl;
#endif

	// cout << "l2 computation done" << endl;

#if DEBUG
	cout << "Reformed dataset ->" << endl;
	for (int i = 0; i < newdb.size(); i++) {
		cout << l2[i][0] << " " << l2[i][1] << " = ";
		set<int>::iterator it;
		for (it = newdb[i].begin(); it != newdb[i].end(); it++) {
			cout << *it << " ";
		}
		cout << endl;
	}
#endif

	vector<int> indexes;
	int offset = 0;

	start = std::chrono::high_resolution_clock::now();

	indexes.push_back(0);
	for (int i = 0; i < l2.size() - 1; i++) {
		bool all_same = true;
		for (int j = 0; j <= offset; j++) {
			if (l2[i][j] != l2[i + 1][j]) {
				all_same = false;
				break;
			}
		}
		if (!all_same) {
			indexes.push_back(i + 1);
		}
	}
	indexes.push_back(l2.size());

	elapsed = std::chrono::high_resolution_clock::now() - start;
	total_time += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

	vector<pthread_t> tids(indexes.size() - 1);

	pthread_mutex_init(&lock, NULL);

	start = std::chrono::high_resolution_clock::now();

#if DEBUG
	cout << "Thread count = " << tids.size() << endl;
#endif

	for (int i = 0; i < tids.size(); i++) {
		equivalence_class_indexes eci;
		eci.start = indexes[i];
		eci.end = indexes[i + 1];
		eci.offset = offset;
		ecis.push_back(eci);
	}

	for (int i = 0; i < tids.size(); i++) {
#if DEBUG
		cout << "Start index = " << ecis[i].start << ", end index = " << ecis[i].end << endl;
#endif
		long pos = (long) i;
		pthread_create(&tids[i], NULL, launch_compute, (void *) pos);
	}
	
	for (int i = 0; i < tids.size(); i++) {
		pthread_join(tids[i], NULL);
	}

	elapsed = std::chrono::high_resolution_clock::now() - start;
	total_time += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

	pthread_mutex_destroy(&lock);

	fstream out_file;
	out_file.open(output_file, fstream::out | fstream::trunc);
#if INFO
	cout << "Frequent itemsets ->" << endl;
	cout << "Count = " << all_freq_itemsets.size() << endl;
#endif
	for (int i = 0; i < all_freq_itemsets.size(); i++) {
		int j;
#if INFO
		cout << "{";
#endif
		out_file << "{";
		for (j = 0; j < all_freq_itemsets[i].size() - 1; j++) {
#if INFO
			cout << all_freq_itemsets[i][j] << ", ";
#endif
			out_file << all_freq_itemsets[i][j] << ", ";
		}
#if INFO
		cout << all_freq_itemsets[i][j] << "}\t";
#endif
		out_file << all_freq_itemsets[i][j] << "}" << endl;
	}
#if INFO
	cout << endl;
#endif
	out_file.close();

	cout << "Total parallel execution time (optimal algorithm) = " << total_time << " us" << endl;

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

	all_freq_itemsets_naive.insert(all_freq_itemsets_naive.end(), l2.begin(), l2.end());

#if INFO
	cout << "l2 ->" << endl;
	cout << "Count = " << l2_naive.size() << endl;
	for (int i = 0; i < l2_naive.size(); i++) {
		cout << "{" << l2_naive[i][0] << ", " << l2_naive[i][1] << "}\t";
	}
	cout << endl;
#endif
	
	vector<int> indexes_naive;
	offset = 0;

	start = std::chrono::high_resolution_clock::now();

	indexes_naive.push_back(0);
	for (int i = 0; i < l2_naive.size() - 1; i++) {
		bool all_same = true;
		for (int j = 0; j <= offset; j++) {
			if (l2[i][j] != l2[i + 1][j]) {
				all_same = false;
				break;
			}
		}
		if (!all_same) {
			indexes_naive.push_back(i + 1);
		}
	}
	indexes_naive.push_back(l2.size());

	elapsed = std::chrono::high_resolution_clock::now() - start;
	total_time_naive += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

	vector<pthread_t> tids_naive(indexes_naive.size() - 1);

	pthread_mutex_init(&lock, NULL);

	start = std::chrono::high_resolution_clock::now();

#if DEBUG
	cout << "Thread count = " << tids_naive.size() << endl;
#endif

	ecis.clear();
	for (int i = 0; i < tids.size(); i++) {
		equivalence_class_indexes eci;
		eci.start = indexes[i];
		eci.end = indexes[i + 1];
		eci.offset = offset;
		ecis.push_back(eci);
	}

	for (int i = 0; i < tids_naive.size(); i++) {
#if DEBUG
		cout << "Start index = " << ecis[i].start << ", end index = " << ecis[i].end << endl;
#endif
		long pos = (long) i;
		pthread_create(&tids_naive[i], NULL, launch_compute_naive, (void *) pos);
	}
	
	for (int i = 0; i < tids_naive.size(); i++) {
		pthread_join(tids_naive[i], NULL);
	}

	elapsed = std::chrono::high_resolution_clock::now() - start;
	total_time_naive += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

	pthread_mutex_destroy(&lock);

	fstream out_file_naive;
	out_file_naive.open(output_file_naive, fstream::out | fstream::trunc);
#if INFO
	cout << "Frequent itemsets ->" << endl;
	cout << "Count = " << all_freq_itemsets_naive.size() << endl;
#endif
	for (int i = 0; i < all_freq_itemsets_naive.size(); i++) {
		int j;
#if INFO
		cout << "{";
#endif
		out_file_naive << "{";
		for (j = 0; j < all_freq_itemsets_naive[i].size() - 1; j++) {
#if INFO
			cout << all_freq_itemsets_naive[i][j] << ", ";
#endif
			out_file_naive << all_freq_itemsets_naive[i][j] << ", ";
		}
#if INFO
		cout << all_freq_itemsets_naive[i][j] << "}\t";
#endif
		out_file_naive << all_freq_itemsets_naive[i][j] << "}" << endl;
	}
#if INFO
	cout << endl;
#endif
	out_file_naive.close();

	cout << "Total parallel execution time (naive algorithm) = " << total_time_naive << " us" << endl;

#endif

	// free(db);
}
