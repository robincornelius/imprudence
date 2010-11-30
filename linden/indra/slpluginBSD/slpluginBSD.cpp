
//SORRY this is a bit windows centric currently 

#include "windows.h"
#include <iostream>
#include <queue>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>

#include "stdtypes.h"
#include "lltimer.h"

#include "llpluginmessage.h"
#include "llpluginmessageclasses.h"
#include "llplugininstance.h"
#include "llpluginsharedmemory.h"
#include "llpluginprocesschild.h"
#include "slpluginBSD.h"



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

	WORD wVersionRequested;
    WSADATA wsaData;

	wVersionRequested = MAKEWORD(2, 2);
    WSAStartup(wVersionRequested, &wsaData);


	LLPluginProcessChild *plugin = new LLPluginProcessChild();
	plugin->init(port);

	LLTimer timer;
	timer.start();
	
	while(!plugin->isDone())
	{
		timer.reset();
		plugin->idle();

		F64 elapsed = timer.getElapsedTimeF64();
		F64 remaining = plugin->getSleepTime() - elapsed;

		if(remaining <= 0.0f)
		{
			plugin->pump();
		}
		else
		{
			plugin->sleep(remaining);
		}
	}
	
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

	mInstance = new LLPluginInstance(this);

}

SLP::~SLP()
{
	 DeleteCriticalSection(&mCriticalSection);
	 delete(mInstance);
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
				Sleep(1000);
				cout << "sending hello \n";
				sendMessageToParent(LLPluginMessage(LLPLUGIN_MESSAGE_CLASS_INTERNAL, "hello"));
				mState = STATE_RUN; 
				break;

			case STATE_RUN:
				EnterCriticalSection(&mCriticalSection);
				
				if(!recvQueue.empty())
				{
					std::string msg = recvQueue.front();
					recvQueue.pop();

					LLPluginMessage pmsg;
					pmsg.parse(msg);

					processmessage(pmsg);

				}

				LeaveCriticalSection(&mCriticalSection);

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

	// If iMode!=0, non-blocking mode is enabled.
	u_long iMode=1;
	ioctlsocket(mListenSocket,FIONBIO,&iMode);

	CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)&SLP::messagethread,this,0,NULL);

}

//static
DWORD SLP::messagethread(LPVOID * user_data)
{
	cout << "thread start \n";

	SLP * pthis = (SLP*)user_data;

	pthis->mState = SLP::STATE_SOCKETGO;

	cout << "Socket is go \n";

	while(pthis->mState<SLP::STATE_DONE)
	{
		//anything to write?

		std::string send_msg;

		EnterCriticalSection(&pthis->mCriticalSection);
		bool toSend = false;
		if(!pthis->sendQueue.empty())
		{
			toSend = true;
			send_msg = pthis->sendQueue.front();
			pthis->sendQueue.pop();
		}
		LeaveCriticalSection(&pthis->mCriticalSection);

		if(toSend)
		{
			cout << "Sending message "<<send_msg<<"\n";	
			int len = send(pthis->mListenSocket,send_msg.c_str(),send_msg.length(),0);
			cout << "sent length "<<len;
			int nError=WSAGetLastError();
			if(nError!=WSAEWOULDBLOCK&&nError!=0)
			{
				//meh we are borked
				cout << "send() error "<<WSAGetLastError()<<"\n";

			}
			char meh=0;

			len = send(pthis->mListenSocket,&meh,1,0);
		}

		char buf[1024]; // urrrg static buffers
		memset(buf,0,1024);

		int len = recv(pthis->mListenSocket,buf,1024,0); ///baaaaag blocking fix me

		if(len==0)
		{
			cout<<"recv() returned 0, parent has closed socket, we go bye bye\n";
			pthis->mState = SLP::STATE_DONE;
			return 0;
		}

		int nError=WSAGetLastError();

		if(nError!=WSAEWOULDBLOCK&&nError!=0)
		{
			//meh we are borked
			cout << "recv() error "<<WSAGetLastError()<<"\n";
			pthis->mState = SLP::STATE_DONE;
			return 0;
		}
		else
		{
			if(len>0)
			{
				cout<<"Data recv() len "<<len<<"content -- "<<buf<<"\n\n";

				EnterCriticalSection(&pthis->mCriticalSection);
				pthis->recvQueue.push(std::string(buf));
				LeaveCriticalSection(&pthis->mCriticalSection);
			}
		}
		
	}

	return 0;
}



void SLP::sendMessageToParent(LLPluginMessage &msg)
{
	cout << "Got message to send\n";
	EnterCriticalSection(&mCriticalSection);
	cout << "queue message\n";
	sendQueue.push(msg.generate());
	LeaveCriticalSection(&mCriticalSection);

}

void SLP::processmessage(LLPluginMessage msg)
{

	if(msg.getClass()==LLPLUGIN_MESSAGE_CLASS_INTERNAL)
	{

		std::string message_name = msg.getName();
		if(message_name == "load_plugin")
		{
			mPluginFile = msg.getValue("file");
			mInstance->load(mPluginFile);
		}
		else if(message_name == "shm_add")
		{
				std::string name = msg.getValue("name");
				size_t size = (size_t)msg.getValueS32("size");
				
				sharedMemoryRegionsType::iterator iter = mSharedMemoryRegions.find(name);
				if(iter != mSharedMemoryRegions.end())
				{
					// Need to remove the old region first
					//LL_WARNS("Plugin") << "Adding a duplicate shared memory segment!" << LL_ENDL;
				}
				else
				{
					// This is a new region
					LLPluginSharedMemory *region = new LLPluginSharedMemory;
					if(region->attach(name, size))
					{
						mSharedMemoryRegions.insert(sharedMemoryRegionsType::value_type(name, region));
						
						std::stringstream addr;
						addr << region->getMappedAddress();
						
						// Send the add notification to the plugin
						LLPluginMessage message("base", "shm_added");
						message.setValue("name", name);
						message.setValueS32("size", (S32)size);
						message.setValuePointer("address", region->getMappedAddress());
						sendMessageToPlugin(message);
						
						// and send the response to the parent
						message.setMessage(LLPLUGIN_MESSAGE_CLASS_INTERNAL, "shm_add_response");
						message.setValue("name", name);
						sendMessageToParent(message);
					}
					else
					{
						//LL_WARNS("Plugin") << "Couldn't create a shared memory segment!" << LL_ENDL;
						delete region;
					}
				}
				
			}
			else if(message_name == "shm_remove")
			{
				std::string name = msg.getValue("name");
				sharedMemoryRegionsType::iterator iter = mSharedMemoryRegions.find(name);
				if(iter != mSharedMemoryRegions.end())
				{
					// forward the remove request to the plugin -- its response will trigger us to detach the segment.
					LLPluginMessage message("base", "shm_remove");
					message.setValue("name", name);
					sendMessageToPlugin(message);
				}
				else
				{
					//LL_WARNS("Plugin") << "shm_remove for unknown memory segment!" << LL_ENDL;
				}
			}
			else if(message_name == "sleep_time")
			{
				mSleepTime = msg.getValueReal("time");
			}
			else if(message_name == "crash")
			{
				// Crash the plugin
				//LL_ERRS("Plugin") << "Plugin crash requested." << LL_ENDL;
				int * p =0;
				(*p)=1;
			}
			else if(message_name == "hang")
			{
				// Hang the plugin
				//LL_WARNS("Plugin") << "Plugin hang requested." << LL_ENDL;
				while(1)
				{
					// wheeeeeeeee......
				}
			}


	}
}

void SLP::receivePluginMessage(const std::string &message)
{

		// Incoming message from the plugin instance
		bool passMessage = true;

		LLPluginMessage parsed;
		parsed.parse(message);
		
		if(parsed.hasValue("blocking_request"))
		{
			mBlockingRequest = true;
		}

		std::string message_class = parsed.getClass();
		if(message_class == "base")
		{
			std::string message_name = parsed.getName();
			if(message_name == "init_response")
			{
				// The plugin has finished initializing.
				//setState(STATE_RUNNING);

				// Don't pass this message up to the parent
				passMessage = false;
				
				LLPluginMessage new_message(LLPLUGIN_MESSAGE_CLASS_INTERNAL, "load_plugin_response");
				LLSD versions = parsed.getValueLLSD("versions");
				new_message.setValueLLSD("versions", versions);
				
				if(parsed.hasValue("plugin_version"))
				{
					std::string plugin_version = parsed.getValue("plugin_version");
					new_message.setValueLLSD("plugin_version", plugin_version);
				}

				// Let the parent know it's loaded and initialized.
				sendMessageToParent(new_message);
			}
			else if(message_name == "shm_remove_response")
			{
				// Don't pass this message up to the parent
				passMessage = false;

				std::string name = parsed.getValue("name");
				sharedMemoryRegionsType::iterator iter = mSharedMemoryRegions.find(name);				
				if(iter != mSharedMemoryRegions.end())
				{
					// detach the shared memory region
					iter->second->detach();
					
					// and remove it from our map
					mSharedMemoryRegions.erase(iter);
					
					// Finally, send the response to the parent.
					LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_INTERNAL, "shm_remove_response");
					message.setValue("name", name);
					sendMessageToParent(message);
				}
				else
				{
					//LL_WARNS("Plugin") << "shm_remove_response for unknown memory segment!" << LL_ENDL;
				}
			}
		}
	
}


void SLP::sendMessageToPlugin(const LLPluginMessage &message)
{

	//this->mInstance->sendMessage();

}

