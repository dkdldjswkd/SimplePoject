#include <iostream>
using namespace std;

#define MAX_INDEX 1000

struct DATA {
public:
	DATA() { cout << "DATA 0" << endl; }
	DATA(const DATA& tmp) : key(tmp.key), value(tmp.value) { cout << "DATA 1" << endl; }
	DATA(string key, string value) : key(key), value(value) { cout << "DATA 2" << endl; }

public:
	string key;
	string value;
};

struct NODE {
	NODE() {}
	NODE(const DATA& data) : data(data) { }

public:
	NODE* next = NULL;
	DATA data;
};

class Hash_table {
private:
	Hash_table() {
		for (int i = 0; i < MAX_INDEX; i++) {
			hash_table[i] = new NODE();
		}
	}

private:
	NODE* hash_table[MAX_INDEX];

public:
	static Hash_table& get_instanc() {
		static Hash_table* instance = new Hash_table();
		return *instance;
	}

	int get_hash(DATA& data) {
		int sum = 0;
		for (int i = 0; i < MAX_INDEX; i++) {
			sum += data.key[i];
		}
		return sum % MAX_INDEX;
	}

	void add(DATA& data) {
		int hash = get_hash(data);

		NODE* cur = hash_table[hash];

		while (1) {
			// 데이터 중복 검사
			if (cur->data.value == data.value) {
				cout << "이미 있는 데이터" << endl;
				return;
			}

			if (cur->next == NULL) {
				cur->next = new NODE(data);
				return;
			}
			else {
				cur = cur->next;
			}
		}
	}

	string find(DATA& data) {
		int hash = get_hash(data);
		NODE* cur = hash_table[hash];

		for (;;) {
			// 데이터 중복 검사
			if (cur->data.value == data.value) {
				return cur->data.value;
			}

			//마지막 노드라면
			if (cur->next == NULL)
				return "";
			cur = cur->next;
		}
	}
};

int main() {
	DATA a("a", "b");
	DATA a2("a", "af");
	Hash_table::get_instanc().add(a);
	Hash_table::get_instanc().add(a2);
	cout << Hash_table::get_instanc().find(a2);
}