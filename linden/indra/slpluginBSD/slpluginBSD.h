#ifndef _SLPLUGINBSD_H
#define _SLPLUGINBSD_H

#ifdef LL_WINDOWS

#include "winsock2.h"
#include "windows.h"

#endif


class SLP
{
public:
	SLP();
	~SLP();

	int mPort;

	void run();

	enum EState
	{
		STATE_UNINITIALIZED = 0,
		STATE_INITIALIZED,		// init() has been called
		STATE_SOCKETGO,	// listening for incoming connection
		STATE_SEND_HELLO,
		STATE_WAIT_HELLO,
		STATE_EXITING,			// Tried to kill process, waiting for it to exit
		STATE_DONE				//

	};
	EState mState;

	SOCKET mListenSocket;  //FIXME for unixoids
	SOCKET mConnectSocket;  //FIXME for unixoids

	std::queue<std::string> recvQueue;
	std::queue<std::string> sendQueue;

	CRITICAL_SECTION mCriticalSection;


private:

	void setupSocket();
	static DWORD messagethread(LPVOID * user_data);

	


};





#endif