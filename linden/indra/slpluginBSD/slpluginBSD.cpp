
//SORRY this is a bit windows centric currently 

#include <iostream>
#include <queue>
#include "slpluginBSD.h"

#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>


using namespace std;


#ifdef LL_WINDOWS
void RedirectIOToConsole();


int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	RedirectIOToConsole();
	cout << "SLPluginBSD launching\n";

	if( strlen( lpCmdLine ) == 0 )
	{	
		cout << "Error invalid start up arguments\nUsage SLPlugin launcherport\n";
	};

	char * end;
	int port = strtol(lpCmdLine, &end, 10);

	SLP instance;
	instance.mPort=port;
	instance.run();
	Sleep(30000); //debug pause so we can read the console


}

static const WORD MAX_CONSOLE_LINES = 500;

void RedirectIOToConsole()
{

int hConHandle;
long lStdHandle;

CONSOLE_SCREEN_BUFFER_INFO coninfo;

FILE *fp;

// allocate a console for this app

AllocConsole();

// set the screen buffer to be big enough to let us scroll text

GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),

&coninfo);

coninfo.dwSize.Y = MAX_CONSOLE_LINES;

SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),

coninfo.dwSize);

// redirect unbuffered STDOUT to the console

lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);

hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

fp = _fdopen( hConHandle, "w" );

*stdout = *fp;

setvbuf( stdout, NULL, _IONBF, 0 );

// redirect unbuffered STDIN to the console

lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);

hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

fp = _fdopen( hConHandle, "r" );

*stdin = *fp;

setvbuf( stdin, NULL, _IONBF, 0 );

// redirect unbuffered STDERR to the console

lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);

hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

fp = _fdopen( hConHandle, "w" );

*stderr = *fp;

setvbuf( stderr, NULL, _IONBF, 0 );

// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog

// point to console as well

ios::sync_with_stdio();

}


#else
int main(int argc, char **argv)
{
	cout << "SLPluginBSD launching\n";

	if(argc!=2)
	{
		cout << "Error invalid start up arguments\nUsage SLPlugin launcherport\n";
		return -1;
	}

	char * end;
	int port = strtol(argv[1], &end, 10);

	cout << "Launcher port is "<<port;

	SLP instance;
	instance.mPort=port;
	instance.run();
}

#endif



SLP::SLP()
{
	mState = STATE_UNINITIALIZED;
	InitializeCriticalSection(&mCriticalSection);

	WORD wVersionRequested;
    WSADATA wsaData;

	wVersionRequested = MAKEWORD(2, 2);
    WSAStartup(wVersionRequested, &wsaData);

}

SLP::~SLP()
{
	 DeleteCriticalSection(&mCriticalSection);
}

void SLP::run()
{
	while(mState != STATE_DONE)
	{	
		switch(mState)
		{
			case STATE_UNINITIALIZED:
				setupSocket();
				mState = STATE_INITIALIZED; 
				break;
		
			case STATE_SOCKETGO:	
				break;

			case STATE_WAIT_HELLO:
				break;
		}

	}
}

void SLP::setupSocket()
{
	// Set up a listening socket

	mListenSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	sockaddr_in service;
	service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
	service.sin_port = htons(this->mPort);

	if(connect(mListenSocket,(SOCKADDR*)&service,sizeof(service))==SOCKET_ERROR)
	{
		mState=STATE_DONE;
		cout << "Error binding socket "<<WSAGetLastError()<<"\n";
		return;
	}

	cout << "Socket is connected to parent.... \n";

	CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)&SLP::messagethread,this,0,NULL);

}

//static
DWORD SLP::messagethread(LPVOID * user_data)
{
	SLP * pthis = (SLP*)user_data;

	pthis->mState = SLP::STATE_SOCKETGO;

	while(pthis->mState<SLP::STATE_DONE)
	{
		//anything to write?

		std::string send_msg;

		EnterCriticalSection(&pthis->mCriticalSection);
		
		if(!pthis->sendQueue.empty())
		{
			send_msg = pthis->sendQueue.front();
			pthis->sendQueue.pop();
		}
		LeaveCriticalSection(&pthis->mCriticalSection);

		send(pthis->mListenSocket,send_msg.c_str(),send_msg.length(),0);

		char buf[1024]; // urrrg static buffers

		int len = recv(pthis->mListenSocket,buf,1024,0); ///baaaaag blocking fix me

		cout<<"Data recv() len "<<len<<"content -- "<<buf<<"\n\n";

		if(len==0)
		{
			cout<<"recv() returned 0, parent has closed socket, we go bye bye\n";
			pthis->mState = SLP::STATE_DONE;
			return 0;
		}

		if(len==SOCKET_ERROR)
		{
			//meh we are borked
			cout << "recv() error "<<WSAGetLastError()<<"\n";
			pthis->mState = SLP::STATE_DONE;
			return 0;
		}
		else
		{
			EnterCriticalSection(&pthis->mCriticalSection);
			pthis->recvQueue.push(std::string(buf));
			LeaveCriticalSection(&pthis->mCriticalSection);
		}
		
	}

	return 0;
}
