#include "dataset.h"
#include "seqalgo.h"

using namespace std;

string output_file = "frequent_itemsets.txt";

tuple<vector<vector<int>>, vector<set<int>>, long long> compute_li(vector<vector<int>> li, vector<set<int>> litxids, int level) {
	vector<int> indexes;
	int offset = 0;
	long long time = 0;
	
	auto start = std::chrono::high_resolution_clock::now();

	indexes.push_back(0);
	for (int i = 0; i < li.size() - 1; i++) {
		if (li[i][offset] != li[i + 1][offset]) {
			indexes.push_back(i + 1);
		}
	}
	indexes.push_back(li.size());

	vector<vector<int>> li_next;
	vector<set<int>> litxids_next;
	for (int i = 0; i < indexes.size() - 1; i++) {
		for (int j = indexes[i]; j < indexes[i + 1] - 1; j++) {
			set<int> txids;
			set_intersection(litxids[j].begin(), litxids[j].end(), litxids[j + 1].begin(), litxids[j + 1].end(), inserter(txids, txids.begin()));
			if (txids.size() >= THRESHOLD) {
				vector<int> items;
				set_union(li[j].begin(), li[j].end(), li[j + 1].begin(), li[j + 1].end(), back_inserter(items));
				li_next.push_back(items);
				litxids_next.push_back(txids);
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

int main() {
	entry *db = (entry *) malloc(NUM_TX * sizeof(entry));

	generate_dataset(db, NUM_TX, NUM_ITEMS);

#if DEBUG
	print_dataset(db);
#endif

	vector<string> all_freq_itemsets;
	long long total_time = 0;

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

	for (int i = 0; i < l1.size(); i++) {
		all_freq_itemsets.push_back("{" + to_string(l1[i]) + "}");
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
	for (int i = 3; i <= LEVEL; i++) {
		if (li.size() >= 1) {
			tuple<vector<vector<int>>, vector<set<int>>, long long> ret = compute_li(li, litxids, i);
			li = get<0>(ret);
			litxids = get<1>(ret);
			total_time += get<2>(ret);
			for (int j = 0; j < li.size(); j++) {
				stringstream items_stream;
				copy(li[j].begin(), li[j].end(), ostream_iterator<int>(items_stream, ", "));
				all_freq_itemsets.push_back("{" + items_stream.str().substr(0, items_stream.str().size() - 2) + "}");
			}
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

	cout << "Total execution time = " << total_time << " us" << endl;

	return 0;
}
