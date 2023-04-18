#pragma once
#include "Define.h"
#include "Session.h"
#include "../../J_LIB/ProtocolBuffer/ProtocolBuffer.h"

class Server {
public:
	static void StartUp();
	static void CleanUp();
	static bool Bind_IOCP(SOCKET h_file, ULONG_PTR completionKey);
	static bool Send_Packet(SESSION_ID session_id, ProtocolBuffer* send_packet);
};

// Set
static bool Create_IOCP();

// 스레드
static void WorkerFunc();
static void AcceptFunc();

// IO 완료 통지 시
static bool Recv_Completion(Session* p_session);
static void Send_Completion(Session* p_session);

// Send/Recv
static bool Post_Recv(Session* p_session);
static bool Post_Send(Session* p_session);
static int	IOCP_Send(Session* p_session, int* io_size);
static int	IOCP_Recv(Session* p_session);
static int	SocketError_Handling(Session* p_session, IO_TYPE io_type);

// 컨텐츠
static void OnRecv(SESSION_ID session_id, ProtocolBuffer* contents_packet);
static void Release_Session(Session* p_session);

// Session
SESSION_ID	Get_SessionID();