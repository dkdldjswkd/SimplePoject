#include <iostream>
using namespace std;

int main() {
	unsigned char a = 9;
	printf("%d�� ���̳ʸ� : ", a);

	for (int i = 0; i < 8; i++) {
		if ((a & 128) == 128) {
			printf("1");
		}
		else {
			printf("0");
		}

		a = (a << 1);
	}
	printf("\n");
}

