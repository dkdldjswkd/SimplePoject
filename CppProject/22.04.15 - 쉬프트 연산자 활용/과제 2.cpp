#include <iostream>
using namespace std;


#define TOP_BIT 32768

int main() {
	unsigned short a = 0;
	int tmp;
	int tmp2;
	int tmp_a;

	while (1) {
		cout << "비트위치 : ";
		cin >> tmp;

		cout << "OFF/ON [0,1] : ";
		cin >> tmp2;

		int bit_n = 1 << (tmp - 1);

		if (tmp2 == 1) {
			a = a | bit_n;
		}
		else {
			a = a & (~bit_n);
		}
		
		tmp_a = a;

		for (int i = 0; i < 16; i++) {
			if ((a & TOP_BIT) == TOP_BIT) {
				printf("%d 번 Bit : ON", 16 - i);
			}
			else {
				printf("%d 번 Bit : OFF", 16 - i);
			}
			printf("\n");

			a = a << 1;
		}
		printf("\n");

		a = tmp_a;
	}
}