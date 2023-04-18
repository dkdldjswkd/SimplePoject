#include <windows.h>
#include <iostream>
#include "Profile.h"
//#include <Windows.h>

using namespace std;

//LARGE_INTEGER Start;
//LARGE_INTEGER End;
//LARGE_INTEGER Freq;

long long test_code;


int main() {
	PRO_BEGIN(L"test");
	Sleep(5000);
	PRO_END(L"test");
	ProfileDataOutText(L"abvc.txt");
}