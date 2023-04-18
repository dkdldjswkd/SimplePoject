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
	long flag_array; // ���������� ��� ����. (�迭�ÿ���)
	WCHAR profile_name[64]; // �������� ���� �̸�.

	LARGE_INTEGER begin_time; // �������� ���� ���� �ð�.

	__int64 total_time; // ��ü ���ð� ī���� Time. (��½� ȣ��ȸ���� ������ ��� ����)
	__int64 min_time[2]; // �ּ� ���ð� ī���� Time. (�ʴ����� ����Ͽ� ���� / [0] �����ּ� [1] ���� �ּ� [2])
	__int64 max_time[2]; // �ִ� ���ð� ī���� Time. (�ʴ����� ����Ͽ� ���� / [0] �����ִ� [1] ���� �ִ� [2])

	__int64 call_count; // ���� ȣ�� Ƚ��.

	LARGE_INTEGER start_time;
	LARGE_INTEGER end_time;
	LARGE_INTEGER freq;
};

#define PROFILE_MAX 100
extern Profiler profile_infos[PROFILE_MAX];
extern unsigned short profile_num;

bool RegisterFunc(const WCHAR* szName, Profiler** info);
void ProfileBegin(const WCHAR* szName); //szName�±��� PROFILE_SAMPLE�� ���ٸ� [���� + �ð�üũ ON], �ִٸ� [�ð�üũ ON]
void ProfileEnd(const WCHAR* szName); // szName�±� PROFILE_SAMPLE�� ã�Ƽ� �ð�üũ OFF
void ProfileDataOutText(const WCHAR* szFileName); // ��ϵ� PROFILE DATA ���� ��� (Name, Average, Min, Max, Call�� fwrite)
void ProfileReset(void); // ��ϵ� PROFILE DATA ���� �ʱ�ȭ
