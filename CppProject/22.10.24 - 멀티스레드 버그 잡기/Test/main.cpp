#include <windows.h>
#include <stdio.h>
#include <process.h>

int a = 0;
unsigned __stdcall f1(void*) {
    for (;;) {
        ++a;
        ++a;
    }
    return 0;
}

int b = 0;
unsigned __stdcall f2(void*) {
    for (;;) {
        ++b;
        ++b;
    }
    return 0;
}

int c = 0;
void f3() {
    for (;;) {
        ++c;
        ++c;
    }
}

unsigned id_f1;
unsigned id_f2;
int main(){

    HANDLE h0 = (HANDLE)_beginthreadex(NULL, 0, &f1, NULL, 0, &id_f1);
    HANDLE h1 = (HANDLE)_beginthreadex(NULL, 0, &f2, NULL, 0, &id_f2);

    f3();

    WaitForSingleObject(h0, INFINITE);
    WaitForSingleObject(h1, INFINITE);
}