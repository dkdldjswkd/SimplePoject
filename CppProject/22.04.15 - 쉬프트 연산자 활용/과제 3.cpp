#include <iostream>
using namespace std;

int main() {
    unsigned int a = 0;

    while (1) {
        cout << "��ġ (1~4) : ";
        int n;
        cin >> n;

        if (n > 5 || n < 1) {
            cout << "��ġ ������ �ʰ��Ͽ����ϴ�.";
            continue;
        }

        cout << "�� [0~255] : ";
        int n2;
        cin >> n2;
        if (n2 > 255 || n < 0) {
            cout << "�� ������ �ʰ��Ͽ����ϴ�.";
            continue;
        }

        //����Ʈ ���� �ڵ�
        int reset_bit = 0x000000ff;
        reset_bit = ~(reset_bit << (n - 1) * 8);
        a = a & reset_bit;

        //�� ���� �ڵ�
        n2 = n2 << (n - 1) * 8;
        a = a | n2;

        int mask_bit = 0x000000ff;
        for (int i = 0; i < 4; i++) {
            printf("%d ��° ����Ʈ �� : %d\n", i + 1, (a & mask_bit) >> i * 8);
            mask_bit = mask_bit << 8;
        }

        printf("��ü 4����Ʈ �� : 0x%p\n\n", a);
    }
}