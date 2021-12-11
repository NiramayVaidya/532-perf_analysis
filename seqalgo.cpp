#include "dataset.h"

#include <vector>
#include <tuple>
#include <map>

using namespace std;

int len = 5;
int total_items = 5;
int threshold = 2;
entry *db;

vector<int> l1;
vector<tuple<int, int>> l2;
map<tuple<int, int>, vector<int>> newdb;

int main() {
	db = (entry *) malloc(len * sizeof(entry));

	generate_dataset(db, len, total_items);
	print_dataset(db);

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

	cout << "l1 ->" << endl;
	for (int i = 0; i < l1.size(); i++) {
		cout << l1[i] << "\t";
	}
	cout << endl;

	for (int i = 0; i < l1.size(); i++) {
		for (int j = i + 1; j < l1.size(); j++) {
			int count = 0;
			vector<int> txids;
			for (int k = 0; k < len; k++) {
				if (db[k].item_present[l1[i] - 1] == 1 && db[k].item_present[l1[j] - 1] == 1) {
					txids.push_back(k);
					count++;
				}
			}
			if (count >= threshold) {
				tuple<int, int> itemset;
				itemset = make_tuple(l1[i], l1[j]);
				l2.push_back(itemset);
				newdb[itemset] = txids;
			}
		}
	}

	cout << "l2 ->" << endl;
	for (int i = 0; i < l2.size(); i++) {
		cout << get<0>(l2[i]) << get<1>(l2[i]) << "\t";
	}
	cout << endl;

	cout << "Reformed dataset ->" << endl;
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

	return 0;
}
