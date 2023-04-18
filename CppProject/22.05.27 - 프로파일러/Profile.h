#pragma once
#include <Windows.h>
#include <profileapi.h>
#include <string.h>
#include <iostream>

using namespace std;

#define PROFILE

#ifdef PROFILE
#define PRO_BEGIN(TagName) ProfileBegin(TagName)
#define PRO_END(TagName) ProfileEnd(TagName)
#else
#define PRO_BEGIN(TagName)
#define PRO_END(TagName)
#endif

struct Profiler
{
	long flag_array; // 프로파일의 사용 여부. (배열시에만)
	WCHAR profile_name[64]; // 프로파일 샘플 이름.

	LARGE_INTEGER begin_time; // 프로파일 샘플 실행 시간.

	__int64 total_time; // 전체 사용시간 카운터 Time. (출력시 호출회수로 나누어 평균 구함)
	__int64 min_time[2]; // 최소 사용시간 카운터 Time. (초단위로 계산하여 저장 / [0] 가장최소 [1] 다음 최소 [2])
	__int64 max_time[2]; // 최대 사용시간 카운터 Time. (초단위로 계산하여 저장 / [0] 가장최대 [1] 다음 최대 [2])

	__int64 call_count; // 누적 호출 횟수.

	LARGE_INTEGER start_time;
	LARGE_INTEGER end_time;
	LARGE_INTEGER freq;
};

#define PROFILE_MAX 100
extern Profiler profile_infos[PROFILE_MAX];
extern unsigned short profile_num;

bool RegisterFunc(const WCHAR* szName, Profiler** info);
void ProfileBegin(const WCHAR* szName); //szName태그의 PROFILE_SAMPLE가 없다면 [생성 + 시간체크 ON], 있다면 [시간체크 ON]
void ProfileEnd(const WCHAR* szName); // szName태그 PROFILE_SAMPLE를 찾아서 시간체크 OFF
void ProfileDataOutText(const WCHAR* szFileName); // 기록된 PROFILE DATA 파일 출력 (Name, Average, Min, Max, Call값 fwrite)
void ProfileReset(void); // 기록된 PROFILE DATA 전부 초기화
