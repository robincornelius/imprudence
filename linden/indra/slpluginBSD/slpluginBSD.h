#ifndef _SLPLUGINBSD_H
#define _SLPLUGINBSD_H

class SLP : public LLPluginInstanceMessageListener
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
		STATE_RUN,
		STATE_EXITING,			// Tried to kill process, waiting for it to exit
		STATE_DONE				//

	};
	EState mState;

	SOCKET mListenSocket;  //FIXME for unixoids
	SOCKET mConnectSocket;  //FIXME for unixoids

	std::queue<std::string> recvQueue;
	std::queue<std::string> sendQueue;

	CRITICAL_SECTION mCriticalSection;

	void receivePluginMessage(const std::string &message);


private:

	void setupSocket();
	static DWORD messagethread(LPVOID * user_data);

	void sendMessageToParent(LLPluginMessage &msg);
	void sendMessageToPlugin(const LLPluginMessage &message);

	void processmessage(LLPluginMessage msg);
	
	std::string mPluginFile;

	typedef void (*sendMessageFunction) (const char *message_string, void **user_data);
	typedef int (*pluginInitFunction) (sendMessageFunction host_send_func, void *host_user_data, sendMessageFunction *plugin_send_func, void **plugin_user_data);
	
	pluginInitFunction mInitFunction;

	static const char *PLUGIN_INIT_FUNCTION_NAME;

	LLPluginInstance * mInstance;

	bool mBlockingRequest;

	typedef std::map<std::string, LLPluginSharedMemory*> sharedMemoryRegionsType;
	sharedMemoryRegionsType mSharedMemoryRegions;

	F64 mSleepTime;

};





#endif