#include "stdafx.h"
#include "Define.h"
#include "Server.h"
#include "Log.h"
#include "protocol.h"
#include "global_var.h"
#include "../../J_LIB/ObjectPool/ObjectPool.h"
#pragma comment(lib, "../../J_LIB/ObjectPool/ObjectPool.lib")
#pragma comment (lib,"../../J_LIB/ProtocolBuffer/ProtocolBuffer.lib")

using namespace std;

//------------------------------
//Server Var
//------------------------------

// 네트워크
SOCKET listen_sock = INVALID_SOCKET;
HANDLE h_iocp = INVALID_HANDLE_VALUE;

// thread
static thread workerThread_Pool[WORKER_NUM];
static thread acceptThread;

// 자료구조
Session* session_array[SESSION_MAX] = { 0, };
stack<DWORD> sessionIndex_stack;

// 오브젝트 풀
ObjectPool<Session> session_pool;
ObjectPool<ProtocolBuffer> protocolBuf_pool;

//------------------------------
// Server Func
//------------------------------

bool Create_IOCP() {
	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, WORKER_NUM);
	return h_iocp != INVALID_HANDLE_VALUE;
}

bool Server::Bind_IOCP(SOCKET h_file, ULONG_PTR completionKey) {
	return h_iocp == CreateIoCompletionPort((HANDLE)h_file, h_iocp, completionKey, 0);
}

void Server::StartUp(){
	WSADATA wsaData;

	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
		throw;

	listen_sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

	if (INVALID_SOCKET == listen_sock)
		throw;

	SOCKADDR_IN	server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (0 != bind(listen_sock, (SOCKADDR*)&server_addr, sizeof(SOCKADDR_IN))) {
		throw;
	}

	if (0 != listen(listen_sock, SOMAXCONN_HINT(65535))) {
		throw;
	}

	// IOCP 객체 생성 (+ 워커 스레드 개수 설정)
	if (!Create_IOCP()) {
		throw;
	}

	// var set
	for (int i = 0; i < SESSION_MAX; i++)
		sessionIndex_stack.push(SESSION_LAST_INDEX - i);

	// Create Thread
	acceptThread = thread(AcceptFunc);
	for (int i = 0; i < WORKER_NUM; i++) {
		workerThread_Pool[i] = thread(WorkerFunc);
	}
}

void Server::CleanUp(void) {
	// WorkerThread 종료
	PostQueuedCompletionStatus(h_iocp, 0, 0, 0);
	for (auto& workerThread : workerThread_Pool) {
		if (workerThread.joinable()) {
			workerThread.join();
		}
	}

	// AcceptThread 종료
	if (acceptThread.joinable()) {
		acceptThread.join();
	}

	// 세션 정리
	for (Session* p_session : session_array) {
		if (p_session == nullptr)
			continue;

		Release_Session(p_session);
	}

	closesocket(listen_sock);
	CloseHandle(h_iocp);
	WSACleanup();
}

void WorkerFunc() {
	printf("Start Worker Thread \n");

	for (;;) {
		DWORD	io_size = 0;
		UINT64	key = 0;
		LPOVERLAPPED p_overlapped = 0;
		Session* p_session = 0;

		BOOL ret_GQCS = GetQueuedCompletionStatus(h_iocp, &io_size, (PULONG_PTR)&key, &p_overlapped, INFINITE);

		// 워커 스레드 종료 (** IO Error X)
		if (io_size == 0 && key == 0 && p_overlapped == 0) {
			PostQueuedCompletionStatus(h_iocp, 0, 0, 0);
			return;
		}

		p_session = (Session*)key;
		
		// FIN
		if (io_size == 0) {
			// 디버깅
			auto ret = WSAGetLastError();
			goto Decrement_IOCount;
		}
		else {
			// recv 완료통지
			if (&p_session->recv_overlapped == p_overlapped) {
				if (FALSE == ret_GQCS) {
					SocketError_Handling(p_session, IO_TYPE::RECV);
					// 디버깅
					printf("[Recv Complete Error] socket(%llu) \n", p_session->sock);
				}
				else {
					printf("[ !!완료통지 RECV ] socket(%llu) \n", p_session->sock);
					p_session->recv_buf.Move_Rear(io_size);
					Recv_Completion(p_session);
				}
			}
			// send 완료통지
			else if (&p_session->send_overlapped == p_overlapped) {
				if (FALSE == ret_GQCS) {
					SocketError_Handling(p_session, IO_TYPE::SEND);
					// 디버깅
					printf("[Send Complete Error] socket(%llu) \n", p_session->sock);
				}
				else {
					printf("[ !!완료통지 SEND ] socket(%llu) \n", p_session->sock);
					p_session->send_buf.Move_Front(io_size);
					Send_Completion(p_session);
				}
			}
			// 디버깅
			else {
				throw;
			}
		}


		Decrement_IOCount:
		if ( InterlockedDecrement( &p_session->io_count ) == 0 ){
			Release_Session(p_session);
			continue;
		}
	}

	printf("End Worker Thread \n");
}

void AcceptFunc() {
	printf("Start Accept Thread \n");

	for (;;) {
		sockaddr_in client_addr;
		int addr_len = sizeof(client_addr);

		//------------------------------
		// Accept
		//------------------------------
		auto accept_sock = accept(listen_sock, (sockaddr*)&client_addr, &addr_len);
		if (accept_sock == INVALID_SOCKET) {
			SocketError_Handling(nullptr, IO_TYPE::ACCEPT);
			break;
		}

		//------------------------------
		// 세션 생성 및 세션 자료구조 등록
		//------------------------------

		// ID 할당
		SESSION_ID session_id = Get_SessionID();
		if (session_id == INVALID_SESSION_ID) {
			closesocket(accept_sock);
			continue;
		}

		// 세션 생성
		Session* p_accept_session = session_pool.Alloc(); 
		p_accept_session->Clear();
		p_accept_session->Set(accept_sock, session_id);

		// 세션 등록
		session_array[session_id.session_index] = p_accept_session;

		// BIND IOCP
		auto ret = Server::Bind_IOCP(accept_sock, (ULONG_PTR)p_accept_session);

		// 디버깅
		INCREMENT_SESSION_COUNT();
		printf("[Accept] socket(%llu) \n", accept_sock);

		//------------------------------
		// WSARecv
		//------------------------------
		ret = Post_Recv(p_accept_session);
	}

	printf("End Accept Thread \n");
}

// 컨텐츠에서 등록하는 콜백함수 (라이브러리 사용자가 등록.)
// 컨텐츠 메시지만 들어있음.
// 컨텐츠 매시지 == 컨텐츠 헤더 + 컨텐츠 페이로드
// 즉, 여기서도 헤더를 까는 등 작업을 해줘야함 but 지금의 에코서버 구조는 그대로 에코하면됨.
void OnRecv(SESSION_ID session_id, ProtocolBuffer* cs_contentsPacket) {
	//------------------------------
	// var set
	//------------------------------
	ProtocolBuffer* sc_contentsPacket = protocolBuf_pool.Alloc();
	sc_contentsPacket->Clear();

	//------------------------------
	// SC Contents Packet 생성
	//------------------------------
	auto cs_contentsPacket_len = cs_contentsPacket->Get_UseSize();
	sc_contentsPacket->Put_Data(cs_contentsPacket->Get_readPos(), cs_contentsPacket_len);
	cs_contentsPacket->Move_Rp(cs_contentsPacket_len);

	//------------------------------
	// Send Packet
	//------------------------------
	Server::Send_Packet(session_id, sc_contentsPacket);

	//------------------------------
	// Release Func
	//------------------------------
	protocolBuf_pool.Free(sc_contentsPacket);
	return;
}

void Release_Session(Session* p_session){
	printf("[세션 끊음] socket(%llu) \n", p_session->sock);

	DECREMENT_SESSION_COUNT();

	closesocket(p_session->sock);
	session_pool.Free(p_session);

	session_array[p_session->session_id.session_index] = nullptr;
	sessionIndex_stack.push(p_session->session_id.session_index);
}

SESSION_ID Get_SessionID(){
	// 0xFFFFFFFF 이 후 Overflow (unique 값 겹칠 수 있음)
	static DWORD unique = 1;

	if (sessionIndex_stack.empty())
		return INVALID_SESSION_ID;
	
	// Index 옮기기
	SESSION_ID session_id(sessionIndex_stack.top(), unique++);
	sessionIndex_stack.pop();

	return session_id;
}

// 네트워크 라이브러리 사용자에게 컨텐츠 패킷을 받고 최종적으로 네트워크 헤더를 랩핑해서 Send
// Send까지 했다면 true, enqueue만 했다면 false
bool Server::Send_Packet(SESSION_ID session_id, ProtocolBuffer* contentsPacket) {
	auto p_session = session_array[session_id.session_index];

	/* LOCK	  */ p_session->lock.lock();
	p_session->send_buf.Enqueue(contentsPacket->Get_readPos(), contentsPacket->Get_UseSize());
	/* UNLOCK */ p_session->lock.unlock();

	Post_Send(p_session);
	return false;
}

bool Recv_Completion(Session* p_session){
	//------------------------------
	// var set
	//------------------------------
	ProtocolBuffer* contents_packet = protocolBuf_pool.Alloc();
	contents_packet->Clear();

	// 디큐잉
	auto recv_size = p_session->recv_buf.Get_UseSize();
	p_session->recv_buf.Dequeue(contents_packet->Get_writePos(), recv_size);
	contents_packet->Move_Wp(recv_size);

	//------------------------------
	// Post Recv (Recv 걸어두기)
	//------------------------------
	auto ret = Post_Recv(p_session);
	if (ret == false) {
		// 디버깅
		throw;

		protocolBuf_pool.Free(contents_packet);
		return false;
	}

	//------------------------------
	// OnRecv (컨텐츠 에서 등록하는 Call Back 함수)
	//------------------------------
	OnRecv(p_session->session_id, contents_packet);

	//------------------------------
	// Release Func
	//------------------------------
	protocolBuf_pool.Free(contents_packet);
	return true;
}

void Send_Completion(Session* p_session){
	InterlockedExchange8((char*)&p_session->send_flag, false);

	if (p_session->send_buf.Empty())
		return;

	Post_Send(p_session);
}

// IO COUNT 증감
bool Post_Recv(Session* p_session){
	InterlockedIncrement((LONG*)&p_session->io_count);

	auto ret = IOCP_Recv(p_session);
	if (SOCKET_ERROR == ret) {
		if (WSAGetLastError() != ERROR_IO_PENDING) {
			printf("[리시브 실패] socket(%llu) \n", p_session->sock);
			if (InterlockedDecrement((LONG*)&p_session->io_count) == 0) {
				Release_Session(p_session);
			}
			return false;
		}
	}

	printf("[리시브 걸어놨다] socket(%llu) \n", p_session->sock);
	return true;
}

bool Post_Send(Session* p_session) {
	if (InterlockedExchange8((char*)&p_session->send_flag, true) == true)
		return false;

	InterlockedIncrement((LONG*)&p_session->io_count);
	int send_size;
	auto ret = IOCP_Send(p_session, &send_size);

	if (SOCKET_ERROR == ret) {
		if (WSAGetLastError() != ERROR_IO_PENDING) {
			printf("[센드 실패] socket(%u) \n", p_session->sock);
			if (InterlockedDecrement((LONG*)&p_session->io_count) == 0) {
				Release_Session(p_session);
			}
			return false;
		}
	}

	printf("[오버랩 센드 했다] socket(%d), send size(%d) \n", p_session->sock, send_size);
	return true;
}

// Send RingBuffer 데이터 전부 Send
int IOCP_Send(Session* p_session, int* io_size) {
	WSABUF wsaBuf[2];

	// Send Remain Pos
	wsaBuf[1].buf = p_session->send_buf.Get_BeginPos();
	wsaBuf[1].len = p_session->send_buf.Remain_DequeueSize();

	// Send Read Pos
	wsaBuf[0].buf = p_session->send_buf.Get_ReadPos();
	wsaBuf[0].len = p_session->send_buf.Direct_DequeueSize();

	//printf("[IOCP_Send] socket(%u),  %d + %d \n", p_session->sock, wsaBuf[0].len, wsaBuf[1].len);

	*io_size = wsaBuf[0].len + wsaBuf[1].len;

	return WSASend(p_session->sock, wsaBuf, 2, NULL, 0, &p_session->send_overlapped, NULL);
}

// Recv RingBuffer에 Recv
int IOCP_Recv(Session* p_session) { 
	WSABUF wsaBuf[2];

	// Recv Remain Pos
	wsaBuf[1].buf = p_session->recv_buf.Get_BeginPos();
	wsaBuf[1].len = p_session->recv_buf.Remain_EnqueueSize();

	// Recv Write Pos
	wsaBuf[0].buf = p_session->recv_buf.Get_WritePos();
	wsaBuf[0].len = p_session->recv_buf.Direct_EnqueueSize();

	//printf("[IOCP_Recv] socket(%u),  %d + %d \n", p_session->sock, wsaBuf[0].len, wsaBuf[1].len);

	DWORD flags = 0;
	return WSARecv(p_session->sock, wsaBuf, 2, NULL, &flags, &p_session->recv_overlapped, NULL);
}

int SocketError_Handling(Session* p_session, IO_TYPE io_type){
	int err_no = WSAGetLastError();

	switch (err_no)
	{
	case 10053:
	case 10054:
		break;

	default:
		Log::Error("[SOCKET ERROR] socket(%d), error no(%d)", p_session->sock, err_no);
	}

	return err_no;
}


// 일단 봉인. 네트워크 헤더 생기면 그때 추가 구현하자

// 반환값 : WSARecv 성공적으로 호출 했는지. (Recv에 실패 혹은 호출하지 않았다면 )
//bool Recv_Completion(Session* p_session){
//	//------------------------------
//	// var set
//	//------------------------------
//	ProtocolBuffer* contents_packet;
//
//	//------------------------------
//	// 데이터 디큐잉 및 패킷 검사 (유효성, 충분한 길이 인지 등)
//	//------------------------------
//	NETWORK_HEADER network_header;
//	int recv_len = p_session->recv_buf.Get_UseSize();
//
//	// 충분한 양의 데이터가 오지 않음 (패킷 헤더조차 도착하지 않음)
//	if (recv_len < NETWORK_HEADER_SIZE) {
//		return Post_Recv(p_session);
//	}
//	// 패킷 검사 (패킷 헤더크기 이상의 데이터 도착)
//	else {
//		p_session->recv_buf.Peek(&network_header, NETWORK_HEADER_SIZE);
//
//		// 충분한 양의 데이터가 오지 않음 (헤더 크기 이상, 패킷 크기 미만)
//		if (recv_len < network_header.contentsPacket_len + NETWORK_HEADER_SIZE) {
//			return Post_Recv(p_session);
//		}
//		// 충분한 양의 데이터를 받음, 데이터 전부 디큐잉 (패킷 조립 가능)
//		else {
//			p_session->recv_buf.Move_Front(NETWORK_HEADER_SIZE);
//			contents_packet = protocolBuf_pool.Alloc();
//			contents_packet->Clear();
//			p_session->recv_buf.Dequeue(contents_packet->Get_writePos(), network_header.contentsPacket_len);
//			contents_packet->Move_Wp(network_header.contentsPacket_len);
//		}
//	}
//
//
//	//------------------------------
//	// Post Recv (Recv 걸어두기)
//	//------------------------------
//	auto ret = Post_Recv(p_session);
//	if (!ret) {
//		// 디버깅
//		throw;
//
//		protocolBuf_pool.Free(contents_packet);
//		return false;
//	}
//
//	//------------------------------
//	// OnRecv (컨텐츠 에서 등록하는 Call Back 함수)
//	//------------------------------
//	OnRecv(p_session->session_id, contents_packet);
//
//	//------------------------------
//	// Release Func
//	//------------------------------
//	protocolBuf_pool.Free(contents_packet);
//	return true;
//}