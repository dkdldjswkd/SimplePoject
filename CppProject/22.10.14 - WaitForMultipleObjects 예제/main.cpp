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

//-과제..
//
//1, WaitForMultipleObjects 를 사용해서 여러개의 스레드 종료 대기.
//
//2.
//- 전역변수 -
//
//int g_Data = 0;
//int g_Connect = 0;
//bool g_Shutdown = false;
//
//
//g_Connect 는 접속자 수를 가상으로 표현.
//g_Data 는 가상의 데이터 처리를 표현.
//
//
//# AcceptThread
//
//100 ~1000ms 마다 랜덤하게 g_Connect 를 + 1
//
//# DisconnectThread
//
//100 ~1000ms 마다 랜덤하게 g_Connect 를 - 1
//
//
//# UpdateThread x 3
//
//10ms 마다 g_Data 를 + 1
//
//그리고 g_Data 가 1000 단위가 될 때 마다 이를 출력
//
//
//# main
//
//1초마다 g_Connect 를 출력
//20초 후 g_Shutdown = true 로 다른 스레드들 종료.
//
//스레드 종료 확인 후 main 종료.
//
//
//
//+beginthreadex 를 사용
//
//+ 모든 변수 및 출력은 동기화가 이루어지도록 안전하게 되어야 함.
//+ interlocked 로 해결되는 경우는 interlocked 사용.
//+ interlocked 로 해결이 안되는 경우는 critical_section 사용.
//
//+ 모든 스레드는 할일이 없는경우 block 상태로 쉬도록 합니다.
//
//+ 모든 스레드는 g_Shutdown 이 true 가 되면 종료됨.
//
//+ main 스레드는 모든 스레드가 종료 된 후에 종료처리.
//
//+ 메인에서 스레드의 종료 확인은 WaitForMultipleObjects 사용.
