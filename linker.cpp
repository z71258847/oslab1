#include <iostream>
#include <cstring>
#include <cmath>
#include <vector>
#include <fstream>
using namespace std;

struct node{
	string key;
	int addr;
}; 

bool add_table(const string& key, const vector<node>& table){
	for (size_t i=0; i<table.size(); i++){
		if (table[i].key == key) return false;
	}
	return true;
}

int main(int argc, char* argv[]) {
	int n;
	cin >> n;
	vector<int> defines;
	vector<int> uses;
	vector<int> instructs;
	vector<node> define_table;
	vector<node> use_table;
	vector<int> instruct_table;
	int cur=0;
	int defined=0;
	for (int f = 0; f < n; f++) {
		int x;
		cin >> x;
		defines.push_back(x);
		for (int i=0; i<defines[f]; i++) {
			string str;
			int add;
			cin >> str >> add;
			add = add + cur;
			if (add_table(str, define_table)) {
				node p;
				p.key = str;
				p.addr = add;
				define_table.push_back(p);
			}
			else {
				cout << "Error: The symbol '" << str << "' has already been defined. It will use the first defined value." << endl;
			}
		}
		
		cin >> x;
		uses.push_back(x);
		for (int i=0; i<uses[f]; i++) {
			string str;
			int add;
			cin >> str >> add;
			node p;
			p.key=str;
			p.addr=add;
			use_table.push_back(p);
		}
		
		cin >> x;
		cur = cur + x;
		instructs.push_back(x);
		for (int i=0; i<instructs[f]; i++) {
			int inst;
			cin >> inst;
			instruct_table.push_back(inst);
		}
		
		for (int i=defined; i<defined+defines[f]; i++) {
			if (define_table[i].addr >= cur) {
				cout << "Error: The symbol '" << define_table[i].key << "' was defined exceeding the size of the module" << f << ". It has been given the address 0(relative)." << endl;
				define_table[i].addr = cur - x;
			}
		}
		defined += defines[f];
		
	}
	
	int ans[cur];
	bool instruct_use[cur];
	bool define_use[define_table.size()];
	int used = 0;
	memset(instruct_use, false, sizeof(instruct_use));
	memset(define_use, false, sizeof(define_use));
	memset(ans, 0, sizeof(ans));
	cur = 0;
	for (int f=0; f<n; f++) {
		//use
		for (int i=0; i<uses[f]; i++) {
			string key = use_table[used+i].key;
			int addr = use_table[used+i].addr;
			int use_addr = -1;
			for (size_t j=0; j<define_table.size(); j++) {
				if (define_table[j].key == key) {
					use_addr = define_table[j].addr;
					define_use[j] = true;
				}
			}
			if (use_addr == -1) {
				cout << "Error: The symbol '" << key << "' was used but not defined. It has been given the value 0." << endl;
				use_addr = 0;
			}
			addr = cur + addr;
			bool list_end = false;
			while (!list_end) {
				int op = instruct_table[addr] / 10000;
				int ex = instruct_table[addr] % 10;
				int temp_add = instruct_table[addr] % 10000 / 10;
				if (ex == 1) cout << "Error: The instruction '" << instruct_table[addr] << "' in module " << f << " was an immediate address but appears on a use list. It has been treated as external" << endl;
				ans[addr] = op*1000+use_addr;
				instruct_use[addr] = true;
				if (instruct_table[addr] % 10000 / 10 == 777) list_end=true;
				addr = cur + temp_add;
			}
		}
		used+=uses[f];
		for (int i=0; i<instructs[f]; i++) {
			int op = instruct_table[cur+i] / 10000;
			int ex = instruct_table[cur+i] % 10;
			int temp_add = instruct_table[cur+i] % 10000 / 10;
			if (!instruct_use[cur+i]) {
				if (ex == 1) ans[cur+i]=op*1000+temp_add;
				else if (ex == 2) ans[cur+i]=op*1000+temp_add;
				else if (ex == 3) ans[cur+i]=op*1000+temp_add+cur;
				else if (ex == 4) {
					cout << "Error: The instruction '" << instruct_table[cur+i] << "' in module " << f << " was an external address but didn't appear on a use list. It has been treated as immediate" << endl;
					ans[cur+i]=op*1000+temp_add;
				}
			}
		}
		cur += instructs[f];
	}
	
	for (int i=0; i< define_table.size(); i++) {
		if (define_use[i] == false) cout <<"Warning: The symbol '" << define_table[i].key << "' was defined but not used." << endl;
	}
	
	cout << "Symbol Table" << endl;
	for (size_t i=0; i<define_table.size(); i++) {
		cout << define_table[i].key << "=" << define_table[i].addr << endl;
	}
	cout << endl;
	
	cout << "Memory Map" << endl;
	for (size_t i=0; i<instruct_table.size(); i++) {
		cout << i << ": " << ans[i] << endl;
	}
}
