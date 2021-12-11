#include "dataset.h"
#include "seqalgo.h"

using namespace std;

int len = 5;
int total_items = 5;
int threshold = 2;
entry *db;

vector<int> l1;
// vector<tuple<int, int>> l2;
// map<tuple<int, int>, vector<int>> newdb;
vector<vector<int>> l2;
vector<set<int>> newdb;

int main() {
	db = (entry *) malloc(len * sizeof(entry));

	generate_dataset(db, len, total_items);

#if DEBUG
	print_dataset(db);
#endif

	for (int i = 0; i < total_items; i++) {
		int count = 0;
		for (int j = 0; j < len; j++) {
			if (db[j].item_present[i] == 1) {
				count++;
			}
		}
		if (count >= threshold) {
			l1.push_back(i + 1);
		}
	}

#if INFO
	cout << "l1 ->" << endl;
	for (int i = 0; i < l1.size(); i++) {
		cout << l1[i] << "\t";
	}
	cout << endl;
#endif

	for (int i = 0; i < l1.size(); i++) {
		for (int j = i + 1; j < l1.size(); j++) {
			int count = 0;
			// vector<int> txids;
			set<int> txids;
			for (int k = 0; k < len; k++) {
				if (db[k].item_present[l1[i] - 1] == 1 && db[k].item_present[l1[j] - 1] == 1) {
					// txids.push_back(k);
					txids.insert(k);
					count++;
				}
			}
			if (count >= threshold) {
				// tuple<int, int> itemset;
				// itemset = make_tuple(l1[i], l1[j]);
				// l2.push_back(itemset);
				// newdb[itemset] = txids;
				vector<int> items;
				items.push_back(l1[i]);
				items.push_back(l1[j]);
				l2.push_back(items);
				newdb.push_back(txids);
			}
		}
	}

#if INFO
	cout << "l2 ->" << endl;
	for (int i = 0; i < l2.size(); i++) {
		// cout << get<0>(l2[i]) << get<1>(l2[i]) << "\t";
		cout << l2[i][0] << l2[i][1] << "\t";
	}
	cout << endl;
#endif

#if DEBUG
	cout << "Reformed dataset ->" << endl;
	/*
	map<tuple<int, int>, vector<int>>::iterator it;
	for (it = newdb.begin(); it != newdb.end(); it++) {
		cout << get<0>(it->first) << get<1>(it->first) << " = {";
		vector<int> txids = it->second;
		int i;
		for (i = 0; i < txids.size() - 1; i++) {
			cout << txids[i] << ", ";
		}
		cout << txids[i] << "}" << endl;
	}
	*/
	for (int i = 0; i < newdb.size(); i++) {
		cout << l2[i][0] << l2[i][1] << " = ";
		set<int>::iterator it;
		for (it = newdb[i].begin(); it != newdb[i].end(); it++) {
			cout << *it << " ";
		}
		cout << endl;
	}
#endif

	if (l2.size() >= 1) {
		vector<int> indexes;
		int offset = 0;
		indexes.push_back(0);
		for (int i = 0; i < l2.size() - 1; i++) {
			if (l2[i][offset] != l2[i + 1][offset]) {
				indexes.push_back(i + 1);
			}
		}
		indexes.push_back(l2.size());

		vector<vector<int>> l3;
		vector<set<int>> l3txids;
		for (int i = 0; i < indexes.size() - 1; i++) {
			for (int j = indexes[i]; j < indexes[i + 1] - 1; j++) {
				set<int> txids;
				set_intersection(newdb[j].begin(), newdb[j].end(), newdb[j + 1].begin(), newdb[j + 1].end(), inserter(txids, txids.begin()));
				if (txids.size() >= threshold) {
					vector<int> items;
					set_union(l2[j].begin(), l2[j].end(), l2[j + 1].begin(), l2[j + 1].end(), back_inserter(items));
					l3.push_back(items);
					l3txids.push_back(txids);
				}
			}
		}
		
#if INFO
		cout << "l3 ->" << endl;
		for (int i = 0; i < l3.size(); i++) {
			cout << l3[i][0] << l3[i][1] << l3[i][2]  << "\t";
		}
		cout << endl;
#endif

#if DEBUG
		cout << "l3 txids ->" << endl;
		for (int i = 0; i < l3txids.size(); i++) {
			cout << l3[i][0] << l3[i][1] << l3[i][2] << " = ";
			set<int>::iterator it;
			for (it = l3txids[i].begin(); it != l3txids[i].end(); it++) {
				cout << *it << " ";
			}
			cout << endl;
		}
#endif
	}

	return 0;
}
