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

HANDLE h[3]; // 0���� Ÿ�Ӿƿ� ��, 1���� shutdown ��, 2���� ���� ��� �̺�Ʈ��

// �̱�
unsigned int PrintThread(void* pArguments) {
    printf("Ready PrintThread \n");

    for (;;) {
        WaitForMultipleObjects(2, h, FALSE, 1000);

        // �˴ٿ�
        if (WAIT_OBJECT_0 == WaitForSingleObject(h[1], 0)) {
            break;
        }

        // Ÿ�Ӿƿ�
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

// �̱�
unsigned int DeleteThread(void* pArguments) {
    printf("Ready DeleteThread \n");

	for (;;) {
        WaitForMultipleObjects(2, h, FALSE, 333);

        // �˴ٿ�
        if (WAIT_OBJECT_0 == WaitForSingleObject(h[1], 0)) {
            break;
        }

        // Ÿ�Ӿƿ�
        AcquireSRWLockExclusive(&lock);
        if(!data_list.empty())
			data_list.pop_back();
        ReleaseSRWLockExclusive(&lock);
	}
	return 1;
}

int seed_num = 0;
// ��Ƽ
unsigned int WorkerThread(void* pArguments) {
    srand(++seed_num);

    printf("Ready WorkerThread \n");

    for (;;) {
        WaitForMultipleObjects(2, h, FALSE, 1000);

        // �˴ٿ�
        if (WAIT_OBJECT_0 == WaitForSingleObject(h[1], 0)) {
            break;
        }

        // Ÿ�Ӿƿ�
        AcquireSRWLockExclusive(&lock);
        data_list.push_back(rand() % 500);
        ReleaseSRWLockExclusive(&lock);
    }
    return 1;
}

// �̱�
unsigned int SaveThread(void* pArguments) {
	FILE* fp;
	fopen_s(&fp, LIST_FILE, "wt");
    if (fp == 0) throw;

    char buf[512];

	printf("Ready SaveThread \n");

    for (;;) {
        WaitForMultipleObjects(3, h, FALSE, INFINITE);

        // �˴ٿ�
        if (WAIT_OBJECT_0 == WaitForSingleObject(h[1], 0)) {
            break;
        }

        // �̺�Ʈ
        buf[0] = 0;

        printf("Start SaveThread \n");

        AcquireSRWLockShared(&lock);
        for (auto iter = data_list.begin();; iter++) {
            if (iter == data_list.end()) {
                // ���� ���
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
                printf("s ������ !! \n");
                SetEvent(h[1]);
                return;
            }
        }
	}
}

int main(){
    //------------------------------
    // �ʱ�ȭ
    //------------------------------
    InitializeSRWLock(&lock);

    //------------------------------
    // ����
    //------------------------------

    THREAD_HANDLE   h_printThread;
    THREAD_ID       id_printThread;

    THREAD_HANDLE   h_deleteThread;
    THREAD_ID       id_deleteThread;

    THREAD_HANDLE   h_saveThread;
	THREAD_ID       id_saveThread;

	THREAD_HANDLE   h_workerThreads[WORKER_THREAD_NUM];
	THREAD_ID       id_workerThreads[WORKER_THREAD_NUM];

	h[0] = CreateEvent(NULL, TRUE, FALSE, NULL); // Ÿ�Ӿƿ� �� (�ñ׳η� �ٲ� ���� x, ������ non-signaled)
	h[1] = CreateEvent(NULL, TRUE, FALSE, NULL); // shut down �� (s �Է� �� signaled)
	h[2] = CreateEvent(NULL, FALSE, FALSE, NULL); // ���� ��� �� (���� �Է� �� signaled)

    //------------------------------
    // ������ ����
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
    // ������ ���
    //------------------------------
    WaitForSingleObject(h_printThread, INFINITE);
    WaitForSingleObject(h_deleteThread, INFINITE);
    WaitForSingleObject(h_saveThread, INFINITE);
    WaitForSingleObject(h_workerThreads[0], INFINITE);
    WaitForSingleObject(h_workerThreads[1], INFINITE);
    WaitForSingleObject(h_workerThreads[2], INFINITE);

    //------------------------------
    // ����
    //------------------------------

    CloseHandle(h_printThread);
    CloseHandle(h_deleteThread);
    CloseHandle(h_saveThread);
    for (int i = 0; i < WORKER_THREAD_NUM; i++)
        CloseHandle(h_workerThreads[i]);
}