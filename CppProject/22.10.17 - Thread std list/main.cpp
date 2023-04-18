#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <process.h>
#include <list>
#include <thread>
using namespace std;

typedef HANDLE      THREAD_HANDLE;
typedef unsigned    THREAD_ID;

#define WORKER_THREAD_NUM 3
#define LIST_FILE "list_data.txt"

SRWLOCK lock;

list<int> data_list;

HANDLE h[3]; // 0번은 타임아웃 용, 1번은 shutdown 용, 2번은 파일 출력 이벤트용

// 싱글
unsigned int PrintThread(void* pArguments) {
    printf("Ready PrintThread \n");

    for (;;) {
        WaitForMultipleObjects(2, h, FALSE, 1000);

        // 셧다운
        if (WAIT_OBJECT_0 == WaitForSingleObject(h[1], 0)) {
            break;
        }

        // 타임아웃
		AcquireSRWLockShared(&lock);
		for (auto iter = data_list.begin();; iter++) {
            if (iter == data_list.end()) {
                printf("end \n");
                ReleaseSRWLockShared(&lock);
                break;
            }

			printf("%d - ", *iter);
        }
    }
    return 1;
}

// 싱글
unsigned int DeleteThread(void* pArguments) {
    printf("Ready DeleteThread \n");

	for (;;) {
        WaitForMultipleObjects(2, h, FALSE, 333);

        // 셧다운
        if (WAIT_OBJECT_0 == WaitForSingleObject(h[1], 0)) {
            break;
        }

        // 타임아웃
        AcquireSRWLockExclusive(&lock);
        if(!data_list.empty())
			data_list.pop_back();
        ReleaseSRWLockExclusive(&lock);
	}
	return 1;
}

int seed_num = 0;
// 멀티
unsigned int WorkerThread(void* pArguments) {
    srand(++seed_num);

    printf("Ready WorkerThread \n");

    for (;;) {
        WaitForMultipleObjects(2, h, FALSE, 1000);

        // 셧다운
        if (WAIT_OBJECT_0 == WaitForSingleObject(h[1], 0)) {
            break;
        }

        // 타임아웃
        AcquireSRWLockExclusive(&lock);
        data_list.push_back(rand() % 500);
        ReleaseSRWLockExclusive(&lock);
    }
    return 1;
}

// 싱글
unsigned int SaveThread(void* pArguments) {
	FILE* fp;
	fopen_s(&fp, LIST_FILE, "wt");
    if (fp == 0) throw;

    char buf[512];

	printf("Ready SaveThread \n");

    for (;;) {
        WaitForMultipleObjects(3, h, FALSE, INFINITE);

        // 셧다운
        if (WAIT_OBJECT_0 == WaitForSingleObject(h[1], 0)) {
            break;
        }

        // 이벤트
        buf[0] = 0;

        printf("Start SaveThread \n");

        AcquireSRWLockShared(&lock);
        for (auto iter = data_list.begin();; iter++) {
            if (iter == data_list.end()) {
                // 파일 출력
                sprintf_s(buf + strlen(buf), sizeof(buf) - strlen(buf), "END \n");
                fwrite(buf, strlen(buf), 1, fp);
                ReleaseSRWLockShared(&lock);
                break;
            }

            sprintf_s(buf + strlen(buf), sizeof(buf) - strlen(buf), "%d - ", *iter);
        }
    }

    fclose(fp);
    return 1;
}

void MainThread() {
	for (;;) {
        if (_kbhit())  {
            if (_getch() == 13) {
				SetEvent(h[2]);
            }
            else if (_getch() == 's' || _getch() == 'S') {
                printf("s 눌럿다 !! \n");
                SetEvent(h[1]);
                return;
            }
        }
	}
}

int main(){
    //------------------------------
    // 초기화
    //------------------------------
    InitializeSRWLock(&lock);

    //------------------------------
    // 변수
    //------------------------------

    THREAD_HANDLE   h_printThread;
    THREAD_ID       id_printThread;

    THREAD_HANDLE   h_deleteThread;
    THREAD_ID       id_deleteThread;

    THREAD_HANDLE   h_saveThread;
	THREAD_ID       id_saveThread;

	THREAD_HANDLE   h_workerThreads[WORKER_THREAD_NUM];
	THREAD_ID       id_workerThreads[WORKER_THREAD_NUM];

	h[0] = CreateEvent(NULL, TRUE, FALSE, NULL); // 타임아웃 용 (시그널로 바꿀 생각 x, 영원히 non-signaled)
	h[1] = CreateEvent(NULL, TRUE, FALSE, NULL); // shut down 용 (s 입력 시 signaled)
	h[2] = CreateEvent(NULL, FALSE, FALSE, NULL); // 파일 출력 용 (엔터 입력 시 signaled)

    //------------------------------
    // 스레드 생성
    //------------------------------

    h_printThread = (HANDLE)_beginthreadex(NULL, 0, &PrintThread, NULL, 0, &id_printThread);
    if (h_printThread == INVALID_HANDLE_VALUE || h_printThread == 0) return 0;

    h_deleteThread = (HANDLE)_beginthreadex(NULL, 0, &DeleteThread, NULL, 0, &id_deleteThread);
	if (h_deleteThread == INVALID_HANDLE_VALUE || h_deleteThread == 0) return 0;

    h_saveThread = (HANDLE)_beginthreadex(NULL, 0, &SaveThread, NULL, 0, &id_saveThread);
    if (h_saveThread == INVALID_HANDLE_VALUE || h_saveThread == 0) return 0;

	for (int i = 0; i < WORKER_THREAD_NUM; i++) {
        h_workerThreads[i] = (HANDLE)_beginthreadex(NULL, 0, &WorkerThread, NULL, 0, &id_workerThreads[i]);
		if (h_workerThreads[i] == INVALID_HANDLE_VALUE || h_workerThreads[i] == 0) return 0;
	}

    MainThread();

    //------------------------------
    // 스레드 대기
    //------------------------------
    WaitForSingleObject(h_printThread, INFINITE);
    WaitForSingleObject(h_deleteThread, INFINITE);
    WaitForSingleObject(h_saveThread, INFINITE);
    WaitForSingleObject(h_workerThreads[0], INFINITE);
    WaitForSingleObject(h_workerThreads[1], INFINITE);
    WaitForSingleObject(h_workerThreads[2], INFINITE);

    //------------------------------
    // 정리
    //------------------------------

    CloseHandle(h_printThread);
    CloseHandle(h_deleteThread);
    CloseHandle(h_saveThread);
    for (int i = 0; i < WORKER_THREAD_NUM; i++)
        CloseHandle(h_workerThreads[i]);
}