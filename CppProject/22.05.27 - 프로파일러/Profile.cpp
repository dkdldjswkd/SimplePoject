#include "Profile.h"

Profiler profile_infos[PROFILE_MAX];
unsigned short profile_num = 0;
FILE* profile_fp;

bool RegisterFunc(const WCHAR* szName, Profiler** info){
	// 이미 등록되어 있는경우
	for (int i = 0; i < profile_num; i++) {
		if (wcscmp(profile_infos[i].profile_name, szName) == 0) {
			*info = &profile_infos[i];
			return true;
		}
	}

	// profiles_infos가 꽉차있음
	if (profile_num >= PROFILE_MAX)
		return false;

	// 등록되어 있지 않은경우 등록함
	wcscpy_s(profile_infos[profile_num].profile_name, 64, szName);
	profile_infos[profile_num].flag_array = true;
	QueryPerformanceFrequency(&profile_infos[profile_num].freq);
	*info = &profile_infos[profile_num++];
	return true;
}

void ProfileBegin(const WCHAR* szName) {
	Profiler* p_profile;
	RegisterFunc(szName, &p_profile);

	QueryPerformanceCounter(&p_profile->start_time);
	p_profile->call_count++;

	// 첫실행시간 체크
	if (p_profile->begin_time.QuadPart == 0) { p_profile->begin_time.QuadPart = p_profile->start_time.QuadPart; }
}

void ProfileEnd(const WCHAR* szName) {
	Profiler* p_profile;
	RegisterFunc(szName, &p_profile);

	QueryPerformanceCounter(&p_profile->end_time);
	long long t = p_profile->end_time.QuadPart - p_profile->start_time.QuadPart;
	p_profile->total_time += t;

	// 0이 더 작게 정렬
	if (p_profile->min_time[1] < p_profile->min_time[0]) {
		auto tmp = p_profile->min_time[0];
		p_profile->min_time[0] = p_profile->min_time[1];
		p_profile->min_time[1] = tmp;
	}
	if (t < p_profile->min_time[1]) {
		p_profile->min_time[1] = t;
	}

	// 0이 더 크게 정렬
	if (p_profile->max_time[0] < p_profile->max_time[1]) {
		auto tmp = p_profile->max_time[0];
		p_profile->max_time[0] = p_profile->max_time[1];
		p_profile->max_time[1] = tmp;
	}
	if (t > p_profile->max_time[1]) {
		p_profile->max_time[1] = t;
	}
}

void ProfileReset(void) {
	for (int i = 0; i < profile_num; i++) {
		profile_infos[profile_num].begin_time.QuadPart	= 0;
		profile_infos[profile_num].total_time			= 0;
		profile_infos[profile_num].min_time[0]				= 0;
		profile_infos[profile_num].min_time[1]				= 0;
		profile_infos[profile_num].max_time[0]				= 0;
		profile_infos[profile_num].max_time[1]				= 0;
		profile_infos[profile_num].call_count				= 0; 
		profile_infos[profile_num].start_time.QuadPart		= 0;
		profile_infos[profile_num].end_time.QuadPart			= 0;
	}
}

void ProfileDataOutText(const WCHAR* szFileName) {
	cout << 1 << endl;

	// 여기서 터짐
	_wfopen_s(&profile_fp, szFileName, L"wt+,ccs=UNICODE");
	cout << 2 << endl;

	if (profile_fp == NULL) {
		printf("profile_fp == NULL \n");
		return;
	}

	//수정해야함
	wchar_t write_buf[500] = {0,};
	for (int i = 0; i < profile_num; i++) {
		wsprintf(write_buf, L"Name : %s / Average : %d / MIN : %d / MAX : %d / Call : %d \n",
			profile_infos[i].profile_name,
			profile_infos[i].total_time / profile_infos[i].call_count,
			profile_infos[i].min_time[0],
			profile_infos[i].max_time[0],
			profile_infos[i].call_count
		);
	}
	auto size = fwrite(write_buf, 1, wcslen(write_buf), profile_fp);
	printf("%d \n", size);

	fclose(profile_fp);
}