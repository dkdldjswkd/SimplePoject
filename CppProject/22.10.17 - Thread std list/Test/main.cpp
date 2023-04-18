#include <iostream>
#include <list>
using namespace std;

list<int> l;

int main() {
	if (!l.empty())
		l.pop_back();
	if (!l.empty())
		l.pop_back();
	if (!l.empty())
		l.pop_back();

	l.push_back(3);

	for (auto iter = l.begin(); iter != l.end(); iter++) {
		printf("%d ", *iter);
	}
}