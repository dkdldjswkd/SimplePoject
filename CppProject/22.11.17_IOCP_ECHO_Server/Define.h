#pragma once

//------------------------------
// Server
//------------------------------
constexpr unsigned WORKER_NUM = 2;
constexpr unsigned PORT = 6000;

enum class IO_TYPE : BYTE {
	NONE,
	SEND,
	RECV,
	ACCEPT,
};

//------------------------------
// Session
//------------------------------
union SESSION_ID {
public:
	struct { DWORD index, unique; } s;
	UINT64	session_id = 0;
#define		session_index  s.index   
#define		session_unique s.unique

public:
	SESSION_ID() {}
	SESSION_ID(DWORD index, DWORD unique_no) { session_index = index, session_unique = unique_no; }
	SESSION_ID(UINT64 value) { session_id = value; }
	~SESSION_ID() {}

public:
	//------------------------------
	// 대입 연산자
	//------------------------------
	void operator=(const SESSION_ID& other) {
		session_id = other.session_id;
	}

	void operator=(UINT64 value) {
		session_id = value;
	}

	bool operator==(UINT64 value) {
		return session_id == value;
	}
};

constexpr UINT64 INVALID_SESSION_ID = 0;
constexpr unsigned SESSION_MAX = 100;
constexpr unsigned SESSION_LAST_INDEX = SESSION_MAX - 1;

//------------------------------
// Protocol
//------------------------------
#define NETWORK_HEADER_SIZE sizeof(NETWORK_HEADER)
