#pragma once

#define MONITORING

#ifdef MONITORING
#define INCREMENT_SESSION_COUNT() do{ InterlockedIncrement((LONG*)&session_count); }while(false)
#define DECREMENT_SESSION_COUNT() do{ InterlockedDecrement((LONG*)&session_count); }while(false)

#endif

#ifndef MONITORING
#define INCREMENT_SESSION_COUNT()
#define DECREMENT_SESSION_COUNT()

#endif // !MONITORING


//------------------------------
// Debug
//------------------------------
extern int session_count;