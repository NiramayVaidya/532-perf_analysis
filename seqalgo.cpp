#include "seqalgo.h"

using namespace std;

static string output_file = "frequent_itemsets.txt";
static string output_file_naive = "frequent_itemsets_naive.txt";

static tuple<vector<vector<int>>, vector<set<int>>, long long> compute_li(vector<vector<int>> li, vector<set<int>> litxids, int offset, int level) {
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
	
	auto elapsed = std::chrono::high_resolution_clock::now() - start;
	time += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

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

	cout << "l" << level << " computation done" << endl;

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

	tuple<vector<vector<int>>, vector<set<int>>, long long> ret;
	ret = make_tuple(li_next, litxids_next, time);
	return ret;
}

#if NAIVE_METHOD
static tuple<vector<vector<int>>, long long> compute_li_naive(vector<vector<int>> li, int offset, int level, entry *db) {
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

	cout << "l" << level << " computation done" << endl;

	tuple<vector<vector<int>>, long long> ret;
	ret = make_tuple(li_next, time);
	return ret;
}
#endif

void seqalgo_run(entry *db) {
	// entry *db = (entry *) malloc(NUM_TX * sizeof(entry));

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
	cout << "Count = " << l1.size() << endl;
	for (int i = 0; i < l1.size(); i++) {
		cout << "{" << l1[i] << "}\t";
	}
	cout << endl;
#endif

	cout << "l1 computation done" << endl;

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
	cout << "Count = " << l2.size() << endl;
	for (int i = 0; i < l2.size(); i++) {
		cout << "{" << l2[i][0] << ", " << l2[i][1] << "}\t";
	}
	cout << endl;
#endif

	cout << "l2 computation done" << endl;

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
	cout << "Count = " << all_freq_itemsets.size() << endl;
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
		cout << "{" << l2_naive[i][0] << ", " << l2_naive[i][1] << "}\t";
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
			tuple<vector<vector<int>>, long long> ret = compute_li_naive(li_naive, offset, i, db);
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
	cout << "Count = " << all_freq_itemsets.size() << endl;
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

	// free(db);
}
