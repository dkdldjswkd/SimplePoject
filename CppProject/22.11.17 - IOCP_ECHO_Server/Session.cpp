#include "stdafx.h"
#include "Session.h"
#include "Session.h"
#pragma comment(lib, "../../J_LIB/RingBuffer/RingBuffer.lib")

//------------------------------
// Session
//------------------------------

Session::Session(){

}

Session::~Session(){

}

void Session::Clear() {
	sock		= INVALID_SOCKET;
	session_id	= INVALID_SESSION_ID;

	send_flag	= false;
	io_count	= 0;
	ZeroMemory(&recv_overlapped, sizeof(recv_overlapped));
	ZeroMemory(&send_overlapped, sizeof(send_overlapped));

	recv_buf.Clear();
	send_buf.Clear();
}

void Session::Set(SOCKET sock, SESSION_ID session_id) {
	this->sock = sock;
	this->session_id = session_id;
}