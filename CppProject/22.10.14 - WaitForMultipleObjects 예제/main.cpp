#include <iostream>
#include <Windows.h>
#include <timeapi.h>
#pragma comment(lib, "Winmm.lib")
#include <process.h>
#include <winnt.h>
using namespace std;

int g_Data = 0;
int g_Connect = 0;
bool g_Shutdown = false;

unsigned int AcceptThread(void*) {
    srand(10);

    printf("start AcceptThread \n");

    for (;;) {
        DWORD wait_t = (rand() % 901) + 100; // 100 ~ 1000

        SleepEx(wait_t, FALSE);
        InterlockedIncrement((unsigned int*) & g_Connect);
    }

    printf("end AcceptThread \n");
    return 1;
}

unsigned int DisconnectThread(void*) {
    srand(20);

    printf("start DisconnectThread \n");

    for (;;) {
        DWORD wait_t = (rand() % 901) + 100; // 100 ~ 1000

        SleepEx(wait_t, FALSE);
        InterlockedDecrement((unsigned int*)&g_Connect);
    }

    printf("end DisconnectThread \n");
    return 1;
}

unsigned int UpdateThread(void*) {
    srand(30);

    printf("start UpdateThread  \n");

    for (;;) {
        auto start_t = timeGetTime();
        for (;;) {
            if (timeGetTime() - start_t > 10) {
                break;
            }
        }

		auto ret_data = InterlockedIncrement((unsigned int*)&g_Data);
		if (ret_data % 1000 == 0) {
			printf("data : %d \n", ret_data);
		}
	}

	printf("end UpdateThread  \n");
    return 1;
}

int main()
{
    HANDLE h[5];
    unsigned int id_accept;
    unsigned int id_disconnect;
    unsigned int id_update[3];

    h[0] = (HANDLE)_beginthreadex(NULL, 0, &AcceptThread, NULL, 0, &id_accept);
    h[1] = (HANDLE)_beginthreadex(NULL, 0, &DisconnectThread, NULL, 0, &id_disconnect);
    h[2] = (HANDLE)_beginthreadex(NULL, 0, &UpdateThread, NULL, 0, &id_update[0]);
    h[3] = (HANDLE)_beginthreadex(NULL, 0, &UpdateThread, NULL, 0, &id_update[1]);
    h[4] = (HANDLE)_beginthreadex(NULL, 0, &UpdateThread, NULL, 0, &id_update[2]);


    for (;;) {
		auto start_t = timeGetTime();
        for (;;) {
            if (timeGetTime() - start_t > 1000) {
                printf("%d \n")
                break;
            }
        }
    }

    WaitForMultipleObjects(5, h, TRUE, INFINITE);

    for (int i = 0; i < 5; i++) {
		CloseHandle(h[i]);
    }
}

//-����..
//
//1, WaitForMultipleObjects �� ����ؼ� �������� ������ ���� ���.
//
//2.
//- �������� -
//
//int g_Data = 0;
//int g_Connect = 0;
//bool g_Shutdown = false;
//
//
//g_Connect �� ������ ���� �������� ǥ��.
//g_Data �� ������ ������ ó���� ǥ��.
//
//
//# AcceptThread
//
//100 ~1000ms ���� �����ϰ� g_Connect �� + 1
//
//# DisconnectThread
//
//100 ~1000ms ���� �����ϰ� g_Connect �� - 1
//
//
//# UpdateThread x 3
//
//10ms ���� g_Data �� + 1
//
//�׸��� g_Data �� 1000 ������ �� �� ���� �̸� ���
//
//
//# main
//
//1�ʸ��� g_Connect �� ���
//20�� �� g_Shutdown = true �� �ٸ� ������� ����.
//
//������ ���� Ȯ�� �� main ����.
//
//
//
//+beginthreadex �� ���
//
//+ ��� ���� �� ����� ����ȭ�� �̷�������� �����ϰ� �Ǿ�� ��.
//+ interlocked �� �ذ�Ǵ� ���� interlocked ���.
//+ interlocked �� �ذ��� �ȵǴ� ���� critical_section ���.
//
//+ ��� ������� ������ ���°�� block ���·� ������ �մϴ�.
//
//+ ��� ������� g_Shutdown �� true �� �Ǹ� �����.
//
//+ main ������� ��� �����尡 ���� �� �Ŀ� ����ó��.
//
//+ ���ο��� �������� ���� Ȯ���� WaitForMultipleObjects ���.
