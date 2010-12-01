
#include "rccommon.h"

#include "llplugininstance.h"

const char *LLPluginInstance::PLUGIN_INIT_FUNCTION_NAME = "LLPluginInitEntryPoint";


std::wstring s2ws(const std::string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}


LLPluginInstance::LLPluginInstance(LLPluginInstanceMessageListener *owner)
{
	mOwner = owner;
}

LLPluginInstance::~LLPluginInstance()
{

}

int LLPluginInstance::load(std::string &plugin_file)
{
	size_t end = plugin_file.find_last_of('\\');
	cout << "Setting dll folder to "<<plugin_file.substr(0,end);
	SetDllDirectory(s2ws(plugin_file.substr(0,end)).c_str());
	mDSOHandle=LoadLibrary(s2ws(plugin_file).c_str());

	if( mDSOHandle==NULL)
	{
		cout << "Failed to load DSO"<< plugin_file<<"\n";
		return -1;
	}

	cout << "DSO loaded successfully\n"; 
	mInitFunction = (pluginInitFunction) GetProcAddress( mDSOHandle,PLUGIN_INIT_FUNCTION_NAME);

	if(mInitFunction)
	{
			cout << "DSO got init function\n"; 
	}
	else
	{
			cout << "DSO FAILED TO FIND INIT hook\n"; 
		return -1;
	}
	
	cout << "trying to init \n"; 

	int result = mInitFunction(staticReceiveMessage, (void*)this, &mPluginSendMessageFunction, &mPluginUserData);

	cout << "init funtion result is"<<result<<"\n"; 

	return 0;

}

//static
void  LLPluginInstance::staticReceiveMessage(const char *message_string, void **user_data)
{
	// TODO: validate that the user_data argument is still a valid LLPluginInstance pointer
	// we could also use a key that's looked up in a map (instead of a direct pointer) for safety, but that's probably overkill
	LLPluginInstance *self = (LLPluginInstance*)*user_data;
	self->receiveMessage(message_string);
}

void LLPluginInstance::receiveMessage(const char *message_string)
{
	if(mOwner)
	{
		//cout << "Incomming message from plugin "<<message_string<<"\n";
		mOwner->receivePluginMessage(message_string);
	}
	else
	{
			cout << "Incomming message with no parent\n";
	}	
}
void LLPluginInstance::idle(void)
{

}

void LLPluginInstance::sendMessage(const std::string &message)
{
	if(mPluginSendMessageFunction)
	{
		//LL_DEBUGS("Plugin") << "sending message to plugin: \"" << message << "\"" << LL_ENDL;
		mPluginSendMessageFunction(message.c_str(), &mPluginUserData);
	}
	else
	{
		//LL_WARNS("Plugin") << "dropping message: \"" << message << "\"" << LL_ENDL;
	}
}

