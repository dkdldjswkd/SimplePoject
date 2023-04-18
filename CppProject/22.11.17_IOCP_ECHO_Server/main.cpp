#include "stdafx.h"
#include "Server.h"
#include "global_var.h"
#include "Log.h"

bool timer(DWORD call_cycle) {
	// var
	static DWORD cur_time = timeGetTime();
	static DWORD prev_time;
	static DWORD cum_time;
	static DWORD delta_time;

	// init time
	prev_time = cur_time;
	cur_time = timeGetTime();
	delta_time = cur_time - prev_time;
	cum_time += delta_time;

	// check cum_time
	if (call_cycle > cum_time)
		return false;
	
	cum_time = 0;
	return true;
}

void server_log() {
	printf("sessoin count : %d \n", session_count);
	printf("\n\n\n\n\n\n\n\n \n\n\n\n\n\n\n\n\n\n \n\n\n\n\n\n\n\n\n\n");
}

int main() {
	Server server;
	server.StartUp();

	Log::SetThreshold(Log::LOG_TYPE_DEBUG);
	Log::Initialise("IOCP ECHO LOG.txt");

	for (;;) {
		Sleep(1000);
		//if (timer(1000)) {
		//	server_log();
		//}
	}

	server.CleanUp();
	return 0;
}

