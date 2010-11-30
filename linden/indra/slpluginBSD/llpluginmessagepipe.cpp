/** 
 * @file llpluginmessagepipe.cpp
 * @brief Classes that implement connections from the plugin system to pipes/pumps.
 *
 * @cond
 * $LicenseInfo:firstyear=2008&license=viewerlgpl$
 * Second Life Viewer Source Code
 * Copyright (C) 2010, Linden Research, Inc.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * version 2.1 of the License only.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
 * $/LicenseInfo$
 * @endcond
 */

//#include "linden_common.h"

#define LL_INFOS(x) std::cout << "WARN " << x << " : "
#define LL_WARNS(x) std::cout << "WARN " << x << " : "
#define LL_DEBUGS(x) std::cout << "DEBUG " << x << " : "
#define LL_ERRS(x) std::cout << "DEBUG " << x << " : " 

#define LL_ENDL "\n";


#include <iostream>
#include <queue>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>

#include "windows.h"

#include "stdtypes.h"

#include "llpluginmessagepipe.h"
//#include "llbufferstream.h"

//#include "llapr.h"

static const char MESSAGE_DELIMITER = '\0';

LLPluginMessagePipeOwner::LLPluginMessagePipeOwner() :
	mMessagePipe(NULL),
	mSocketError(0)
{
}

// virtual 
LLPluginMessagePipeOwner::~LLPluginMessagePipeOwner()
{
	killMessagePipe();
}

// virtual 
int LLPluginMessagePipeOwner::socketError(int error)
{ 
	mSocketError = error;
	return error; 
};

//virtual 
void LLPluginMessagePipeOwner::setMessagePipe(LLPluginMessagePipe *read_pipe)
{
	// Save a reference to this pipe
	mMessagePipe = read_pipe;
}

bool LLPluginMessagePipeOwner::canSendMessage(void)
{
	return (mMessagePipe != NULL);
}

bool LLPluginMessagePipeOwner::writeMessageRaw(const std::string &message)
{
	bool result = true;
	if(mMessagePipe != NULL)
	{
		result = mMessagePipe->addMessage(message);
	}
	else
	{
		LL_WARNS("Plugin") << "dropping message: " << message << LL_ENDL;
		result = false;
	}
	
	return result;
}

void LLPluginMessagePipeOwner::killMessagePipe(void)
{
	if(mMessagePipe != NULL)
	{
		delete mMessagePipe;
		mMessagePipe = NULL;
	}
}

LLPluginMessagePipe::LLPluginMessagePipe(LLPluginMessagePipeOwner *owner, int port):
	mOwner(owner)
{
	mPort = port;
	mOwner->setMessagePipe(this);

	mSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	sockaddr_in service;
	service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
	service.sin_port = htons(port);

	if(connect(mSocket,(SOCKADDR*)&service,sizeof(service))==SOCKET_ERROR)
	{
		LL_ERRS("LLPluginMessagePipe") << "Error binding socket "<<WSAGetLastError()<< LL_ENDL;
		return;
	}

	u_long iMode=1;
	ioctlsocket(mSocket,FIONBIO,&iMode);

	LL_INFOS("LLPluginMessagePipe")<<"Socket open handle "<<mSocket<<LL_ENDL;

}

LLPluginMessagePipe::~LLPluginMessagePipe()
{
	LL_INFOS("LLPluginMessagePipe")<<"Deconstruction"<< LL_ENDL;

	if(mOwner != NULL)
	{
		mOwner->setMessagePipe(NULL);
	}
}

bool LLPluginMessagePipe::addMessage(const std::string &message)
{
	// queue the message for later output
	//LLMutexLock lock(&mOutputMutex);
	mOutput += message;
	mOutput += MESSAGE_DELIMITER;	// message separator
	
	return true;
}

void LLPluginMessagePipe::clearOwner(void)
{
	// The owner is done with this pipe.  The next call to process_impl should send any remaining data and exit.
	mOwner = NULL;
}

void LLPluginMessagePipe::setSocketTimeout(int timeout_usec)
{
	
}

bool LLPluginMessagePipe::pump(F64 timeout)
{
//	LL_INFOS("LLPluginMessagePipe")<<"PUMP"<< LL_ENDL;


	bool result = pumpOutput();
	
	if(result)
	{
		result = pumpInput(timeout);
	}
	
	return result;
}

bool LLPluginMessagePipe::pumpOutput()
{
	bool result=true;

	//LL_INFOS("LLPluginMessagePipe")<<"PUMP Output"<< LL_ENDL;


	{
		//RCMutexLock(mOutputMutex);
			
		if(!mOutput.empty())
		{
			std::cout<<"sending "<<mOutput<<"\n";
			int len_sent = send(mSocket,mOutput.data(),mOutput.size(),0);
			if(len_sent!=mOutput.size())
			{
				result=false;
			}

			mOutput = mOutput.substr(len_sent);

			//FIX me more error checking
		}
	}

	return result;
}

bool LLPluginMessagePipe::pumpInput(F64 timeout)
{
	bool result=true;

	//LL_INFOS("LLPluginMessagePipe")<<"Pump inpput"<< LL_ENDL;


	if(timeout != 0.0f)
	{
		Sleep((int)(timeout * 1000.0f));
		timeout = 0.0f;
	}


	char buf[4096]; // urrrg static buffers
	memset(buf,0,4096);

	int len = recv(mSocket,buf,1024,0); 
	
	if(len>0)
	{
		std::cout<<"recv "<<buf<<"\n";
		//RCMutexLock(mInputMutex);
		mInput.append(buf, len);
	}

	processInput();

	return result;	
}

void LLPluginMessagePipe::processInput(void)
{
	// Look for input delimiter(s) in the input buffer.
	int delim;
	//mInputMutex.Lock();
	while((delim = mInput.find(MESSAGE_DELIMITER)) != std::string::npos)
	{	
		// Let the owner process this message
		if (mOwner)
		{
			// Pull the message out of the input buffer before calling receiveMessageRaw.
			// It's now possible for this function to get called recursively (in the case where the plugin makes a blocking request)
			// and this guarantees that the messages will get dequeued correctly.
			std::string message(mInput, 0, delim);
			mInput.erase(0, delim + 1);
			mInputMutex.UnLock();
			mOwner->receiveMessageRaw(message);
			mInputMutex.Lock();
		}
		else
		{
			LL_WARNS("Plugin") << "!mOwner" << LL_ENDL;
		}
	}
	//mInputMutex.UnLock();
}

