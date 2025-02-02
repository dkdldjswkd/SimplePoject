#pragma once
#include "Define.h"
#include <mutex>
#include "../../J_LIB/RingBuffer/RingBuffer.h"

//클라이언트 정보를 담기위한 구조체
class Session {
public:
	Session();
	~Session();

public:
	SOCKET		sock		= INVALID_SOCKET;
	SESSION_ID	session_id	= INVALID_SESSION_ID;

	bool send_flag	= false;
	LONG io_count	= 0;

	OVERLAPPED recv_overlapped = { 0, };
	OVERLAPPED send_overlapped = { 0, };
	RingBuffer recv_buf;
	RingBuffer send_buf;

	std::mutex lock;

public:
	void Clear();
	void Set(SOCKET sock, SESSION_ID session_id);

};

typedef Session* PSession;

