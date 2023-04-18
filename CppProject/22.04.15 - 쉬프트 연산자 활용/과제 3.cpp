#include <iostream>
using namespace std;

int main() {
    unsigned int a = 0;

    while (1) {
        cout << "위치 (1~4) : ";
        int n;
        cin >> n;

        if (n > 5 || n < 1) {
            cout << "위치 범위를 초과하였습니다.";
            continue;
        }

        cout << "값 [0~255] : ";
        int n2;
        cin >> n2;
        if (n2 > 255 || n < 0) {
            cout << "값 범위를 초과하였습니다.";
            continue;
        }

        //바이트 리셋 코드
        int reset_bit = 0x000000ff;
        reset_bit = ~(reset_bit << (n - 1) * 8);
        a = a & reset_bit;

        //값 쓰기 코드
        n2 = n2 << (n - 1) * 8;
        a = a | n2;

        int mask_bit = 0x000000ff;
        for (int i = 0; i < 4; i++) {
            printf("%d 번째 바이트 값 : %d\n", i + 1, (a & mask_bit) >> i * 8);
            mask_bit = mask_bit << 8;
        }

        printf("전체 4바이트 값 : 0x%p\n\n", a);
    }
}