/**
 * 
 * Copyright (c) 2008-2010, Linden Research, Inc.
 * Copyright (c) 2010, Robin Cornelius <robin.cornelius@gmail.com>
 * 
 * Second Life Viewer Source Code
 * The source code in this file ("Source Code") is provided by Linden Lab
 * to you under the terms of the GNU General Public License, version 2.0
 * ("GPL"), unless you have obtained a separate licensing agreement
 * ("Other License"), formally executed by you and Linden Lab.  Terms of
 * the GPL can be found in doc/GPL-license.txt in this distribution, or
 * online at http://secondlife.com/developers/opensource/gplv2
 * 
 * There are special exceptions to the terms and conditions of the GPL as
 * it is applied to this Source Code. View the full text of the exception
 * in the file doc/FLOSS-exception.txt in this software distribution, or
 * online at
 * http://secondlife.com/developers/opensource/flossexception
 * 
 * By copying, modifying or distributing this software, you acknowledge
 * that you have read and understood your obligations described above,
 * and agree to abide by those obligations.
 * 
 * ALL LINDEN LAB SOURCE CODE IS PROVIDED "AS IS." LINDEN LAB MAKES NO
 * WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
 * COMPLETENESS OR PERFORMANCE.
 * $/LicenseInfo$
 * 
 * @endcond
 */

#pragma warning( disable :  4189 )

#include "linden_common.h"

#include "llgl.h"
#include "llplugininstance.h"
#include "llpluginmessage.h"
#include "llpluginmessageclasses.h"
#include "media_plugin_base.h"

#include <vlc/vlc.h>
#include "media_plugin_vlc.h"

#pragma warning( disable :  4189 )

////////////////////////////////////////////////////////////////////////////////
//

MediaPluginVLC * MediaPluginVLC::sInstance;

////////////////////////////////////////////////////////////////////////////////
//
static void *lock(void *data, void **p_pixels)
{
	MediaPluginVLC * ppthis = (MediaPluginVLC *)data;

	if(ppthis->mCurrentInitState==MediaPluginVLC::STATE_WAITFMT)
	{
		unsigned int px=0,py=0;
		libvlc_video_get_size(ppthis->mp,0,&px,&py);
		if(!ppthis->mSizeChangeRequestSent && px>0 && py>0)
		{
			ppthis->mCurrentInitState = MediaPluginVLC::STATE_GOTFMT;
			ppthis->mNaturalWidth = px;
			ppthis->mNaturalHeight = py;
		}	

		*p_pixels = ppthis->mDummyRenderBuffer;
	}
	else
	{
		*p_pixels = ppthis->mRenderBuffer;
		if(ppthis->mCurrentInitState==MediaPluginVLC::STATUS_DANCEFINISHED)
			ppthis->mPlayingForReal = true;
	}

	return NULL;
}

static void unlock(void *data, void *id, void *const *p_pixels)
{
	MediaPluginVLC * ppthis = (MediaPluginVLC *)data;
}

static void display(void *data, void *id)
{
	MediaPluginVLC * ppthis = (MediaPluginVLC *)data;
	ppthis->Invalidate();
}

//static
void MediaPluginVLC::status_callback(const libvlc_event_t *ev, void *data)
{
	MediaPluginVLC * pthis = (MediaPluginVLC *)data;

	switch(ev->type)
	{
		case libvlc_MediaPlayerPlaying:
			pthis->setStatus(STATUS_PLAYING);	
		break;

		case libvlc_MediaPlayerPaused:
			pthis->setStatus(STATUS_PAUSED);	
		break;

		case libvlc_MediaPlayerStopped:
			pthis->setStatus(STATUS_DONE);
		break;

		case libvlc_MediaPlayerEndReached:
			{
				libvlc_media_t *m = libvlc_media_player_get_media(pthis->mp);
				libvlc_media_list_t * sub = libvlc_media_subitems(m);
		
				if(!sub)
				{
					pthis->setStatus(STATUS_DONE);
				}
				else
				{
					int noSubItems = libvlc_media_list_count(sub);
					for(int pos=0;pos<noSubItems;pos++)
					{
						libvlc_media_t * subMedia = libvlc_media_list_item_at_index(sub,pos);
						char * mrl = libvlc_media_get_mrl(subMedia);
						pthis->mMediaList.push_back(std::string(mrl));
						libvlc_media_release (subMedia);
					}
				}

				libvlc_media_release (m);

				pthis->mMoveNextMedia = true;

				break;
			}

		case libvlc_MediaPlayerOpening:
		case libvlc_MediaPlayerBuffering:
			pthis->setStatus(STATUS_LOADING);
		break;

		case libvlc_MediaPlayerEncounteredError:
			pthis->setStatus(STATUS_ERROR);
		break;
	}				
}

MediaPluginVLC::MediaPluginVLC( LLPluginInstance::sendMessageFunction host_send_func, void *host_user_data ) :
	MediaPluginBase( host_send_func, host_user_data )
{
	mLastUpdateTime = 0;
	sInstance=this;
	mWidth=100;
	mHeight=100;
	mDepth=4;
	mCurrentVolume = 0.25;
	mMoveNextMedia = false;
	mTextureWidth = 0;
	mTextureHeight = 0;
	
}

////////////////////////////////////////////////////////////////////////////////
//
MediaPluginVLC::~MediaPluginVLC()
{
	if(mp)
	{
		libvlc_media_player_stop (mp);
		libvlc_media_player_release (mp);
	}
 
	if(inst)
	{
		libvlc_release (inst);
	}
}

////////////////////////////////////////////////////////////////////////////////
//
void  MediaPluginVLC::size_change_request(int w,int h)
{
	LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA, "size_change_request");
	message.setValue("name", mTextureSegmentName);
	message.setValueS32("width", w);
	message.setValueS32("height", h);
	sendMessage(message);

}

////////////////////////////////////////////////////////////////////////////////
//
void MediaPluginVLC::receiveMessage( const char* message_string )
{
	LLPluginMessage message_in;

	if ( message_in.parse( message_string ) >= 0 )
	{
		std::string message_class = message_in.getClass();
		std::string message_name = message_in.getName();

		if ( message_class == LLPLUGIN_MESSAGE_CLASS_BASE )
		{
			if ( message_name == "init" )
			{
				LLPluginMessage message( "base", "init_response" );
				LLSD versions = LLSD::emptyMap();
				versions[ LLPLUGIN_MESSAGE_CLASS_BASE ] = LLPLUGIN_MESSAGE_CLASS_BASE_VERSION;
				versions[ LLPLUGIN_MESSAGE_CLASS_MEDIA ] = LLPLUGIN_MESSAGE_CLASS_MEDIA_VERSION;
				versions[ LLPLUGIN_MESSAGE_CLASS_MEDIA_BROWSER ] = LLPLUGIN_MESSAGE_CLASS_MEDIA_BROWSER_VERSION;
				message.setValueLLSD( "versions", versions );
				
				std::string plugin_version = "VLC plugin, Version 1.0.0.0";

				setStatus(STATUS_NONE);

				char * options[] = {"path","--transform-type=vflip","--vout-filter=transform",NULL};

				inst = libvlc_new (3, options);

				//this can and will fail if it can't find its plugins, default search
				//is for plugins/
				
				if(!inst)
				{
					//Abuse this message to indicate a fail
					plugin_version = "VLC plugin, Version 1.0.0.0 - failed to start VLC core";
				}
		
				message.setValue( "plugin_version", plugin_version );
				sendMessage( message );
			}
			else
			if ( message_name == "idle" )
			{
				// no response is necessary here.
				F64 time = message_in.getValueReal( "time" );

				// Convert time to milliseconds for update()
				update( time );
			}
			else
			if ( message_name == "cleanup" )
			{
			}
			else
			if ( message_name == "shm_added" )
			{
				SharedSegmentInfo info;
				info.mAddress = message_in.getValuePointer( "address" );
				info.mSize = ( size_t )message_in.getValueS32( "size" );
				std::string name = message_in.getValue( "name" );
				mTextureSegmentName = name;
				mSharedSegments.insert( SharedSegmentMap::value_type( name, info ) );
			}
			else
			if ( message_name == "shm_remove" )
			{
				std::string name = message_in.getValue( "name" );

				SharedSegmentMap::iterator iter = mSharedSegments.find( name );
				if( iter != mSharedSegments.end() )
				{
					if ( mPixels == iter->second.mAddress )
					{
						// This is the currently active pixel buffer.
						// Make sure we stop drawing to it.
						mRenderBuffer = mPixels = NULL;
						mTextureSegmentName.clear();
					};
					mSharedSegments.erase( iter );
				}
				else
				{
					//std::cerr << "MediaPluginVLC::receiveMessage: unknown shared memory region!" << std::endl;
				};

				// Send the response so it can be cleaned up.
				LLPluginMessage message( "base", "shm_remove_response" );
				message.setValue( "name", name );
				sendMessage( message );
			}
			else
			{
				//std::cerr << "MediaPluginVLC::receiveMessage: unknown base message: " << message_name << std::endl;
			};
		}
		else
		if ( message_class == LLPLUGIN_MESSAGE_CLASS_MEDIA )
		{
			if ( message_name == "init" )
			{
				// Plugin gets to decide the texture parameters to use.
				LLPluginMessage message( LLPLUGIN_MESSAGE_CLASS_MEDIA, "texture_params" );
				message.setValueS32( "default_width", mWidth );
				message.setValueS32( "default_height", mHeight );
				message.setValueS32( "depth", mDepth );
				message.setValueU32( "internalformat", GL_RGBA );
				message.setValueU32( "format", GL_RGBA );
				message.setValueU32( "type", GL_UNSIGNED_BYTE );
				message.setValueBoolean( "coords_opengl", false );
				sendMessage( message );
			}
			else if ( message_name == "size_change" )
			{
				std::string name = message_in.getValue( "name" );
				S32 width = message_in.getValueS32( "width" );
				S32 height = message_in.getValueS32( "height" );
				S32 texture_width = message_in.getValueS32( "texture_width" );
				S32 texture_height = message_in.getValueS32( "texture_height" );

				std::cerr << "size change message "<<width<<" x "<<height<<" \n";

				if ( ! name.empty() )
				{
					// Find the shared memory region with this name
					SharedSegmentMap::iterator iter = mSharedSegments.find( name );
					if ( iter != mSharedSegments.end() )
					{
						mRenderBuffer = mPixels = ( unsigned char* )iter->second.mAddress;
						mWidth = width;
						mHeight = height;

						mTextureWidth = texture_width;
						mTextureHeight = texture_height;
				
						if(mHeight==mNaturalHeight && mWidth==mNaturalWidth)
						{
							std::cerr << "size change complete\n";
							init();
							if(mCurrentInitState == STATUS_WAITSIZECHANGE)
							{
								mCurrentInitState = STATUS_SIZECHANGECOMPLETE;	
							}
						}
					};
				};
			
				
				LLPluginMessage message( LLPLUGIN_MESSAGE_CLASS_MEDIA, "size_change_response" );
				message.setValue( "name", name );
				message.setValueS32( "width", width );
				message.setValueS32( "height", height );
				message.setValueS32( "texture_width", texture_width );
				message.setValueS32( "texture_height", texture_height );
				sendMessage( message );
			}
			else
			if ( message_name == "load_uri" )
			{
				std::string uri = message_in.getValue( "uri" );

				if ( ! uri.empty() )
				{				
					 mMediaList.clear();
					 LoadURI(uri);
			    }
			}
			else
			{
				//std::cerr << "MediaPluginVLC::receiveMessage: unknown media message: " << message_string << std::endl;
			};
		}
		else if(message_class == LLPLUGIN_MESSAGE_CLASS_MEDIA_TIME)
		{
			if(mp==NULL)
			{
				//media player is dead, ignore
				return;
			}
			if(message_name == "stop")
			{
				 libvlc_media_player_stop (mp);
			}
			else if(message_name == "start")
			{
				 libvlc_media_player_play(mp);
			}
			else if(message_name == "pause")
			{
				libvlc_media_player_pause (mp);	
			}
			else if(message_name == "seek")
			{
				F64 time = message_in.getValueReal("time");

			}
			else if(message_name == "set_loop")
			{
				bool loop = message_in.getValueBoolean("loop");
			}
			else if(message_name == "set_volume")
			{
				F64 volume = message_in.getValueReal("volume");
				libvlc_audio_set_volume(mp,volume*200);
				mCurrentVolume = volume;
			}
		}
		else
		{
			//std::cerr << "MediaPluginVLC::receiveMessage: unknown message class: " << message_class << std::endl;
		};
	};
}

void MediaPluginVLC::LoadURI(std::string uri)
{
	mSizeChangeRequestSent = false;
	mNaturalWidth = 0;
	mNaturalHeight = 0;

	mPlayingForReal = false;

	if(mp)
	{
		libvlc_media_player_stop (mp);
		libvlc_media_player_release (mp);
	}
 
	/* Create a new item */
	libvlc_media_t *m  = libvlc_media_new_path (inst, uri.c_str());
		        
	/* Create a media player playing environement */
	mp = libvlc_media_player_new_from_media (m);
	    
	//Listen to some events we might care about
	em = libvlc_media_player_event_manager(mp);
	libvlc_event_attach(em, libvlc_MediaPlayerPlaying, status_callback, this);
	libvlc_event_attach(em, libvlc_MediaPlayerPaused, status_callback, this);
	libvlc_event_attach(em, libvlc_MediaPlayerStopped, status_callback, this);
	libvlc_event_attach(em, libvlc_MediaPlayerOpening, status_callback, this);
	libvlc_event_attach(em, libvlc_MediaPlayerBuffering, status_callback, this);
	libvlc_event_attach(em, libvlc_MediaPlayerEndReached, status_callback, this);
	libvlc_event_attach(em, libvlc_MediaPlayerEncounteredError, status_callback, this);

	libvlc_video_set_format(mp, "RGBA", 10, 10, 4*10); //size of init buffer
	libvlc_video_set_callbacks(mp, lock, unlock, display, this);
	mCurrentInitState = STATE_WAITFMT;

	libvlc_audio_set_volume(mp,mCurrentVolume*200);

	setStatus(STATUS_LOADING);

	libvlc_media_parse_async(m);

	/* No need to keep the media now */
	libvlc_media_release (m);

}

////////////////////////////////////////////////////////////////////////////////
//
void MediaPluginVLC::update( F64 milliseconds )
{
	if(!mp)
		return;

	static time_t last_clock=0;
	
	if(mPlayingForReal==false)
	{
		for(int y=0;y<mTextureHeight;y++)
		{
			for(int x=0;x<mTextureWidth*4;x=x+4)
			{
				*(mRenderBuffer+(y*mTextureWidth*4)+x)=rand()*255;
				*(mRenderBuffer+(y*mTextureWidth*4)+x+1)=rand()*255;
				*(mRenderBuffer+(y*mTextureWidth*4)+x+2)=rand()*255;
			}
		}
		Invalidate();
	}

	switch(mCurrentInitState)
	{
		case STATE_GOTFMT:
			libvlc_media_player_stop (mp);
			mCurrentInitState = STATE_WAITSTOP;
			break;

		case STATE_WAITSTOP:
			if(mStatus==STATUS_DONE)
			{
				 libvlc_video_set_format(mp, "RGBA", mNaturalWidth, mNaturalHeight, 4*mNaturalWidth); //size of init buffer
				 libvlc_video_set_callbacks(mp, lock, unlock, display, this);
				 if(mNaturalWidth == mTextureWidth && mNaturalHeight == mTextureHeight)
				 {
					mCurrentInitState = STATUS_SIZECHANGECOMPLETE;
				 }
				 else
				 {
					 size_change_request(mNaturalWidth, mNaturalHeight);
					 mCurrentInitState = STATUS_WAITSIZECHANGE;
				 }
			}
			break;
		case STATUS_SIZECHANGECOMPLETE:
			{
				libvlc_media_player_play (mp);
				mCurrentInitState= STATUS_DANCEFINISHED;
			}
	}

	if(clock()>last_clock+CLOCKS_PER_SEC)
	{
		//title poll *sigh* why don't meta events work on streams
		libvlc_media_t *m = libvlc_media_player_get_media(mp);
		if(libvlc_media_is_parsed(m))
		{
			char * nowPlaying = libvlc_media_get_meta(m,libvlc_meta_NowPlaying);
			char * Title = libvlc_media_get_meta(m,libvlc_meta_Title);

			if(nowPlaying && Title)
			{
				if(mNowPlaying.compare(nowPlaying) || mTitle.compare(Title))
				{
					mNowPlaying=nowPlaying;
					mTitle=Title;

					LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA, "name_text");
					message.setValue("name", mNowPlaying+" - "+mTitle);
					sendMessage( message );
				}
			}
		}
	}

	if(mMoveNextMedia && !mMediaList.empty())
	{
		mMoveNextMedia = false;

		std::string url = mMediaList.front();
		mMediaList.pop_front();

		/* Create a new item */
		LoadURI(url);

		libvlc_media_player_play(mp);
	}
};

////////////////////////////////////////////////////////////////////////////////
//
bool MediaPluginVLC::init()
{
	LLPluginMessage message( LLPLUGIN_MESSAGE_CLASS_MEDIA, "name_text" );
	message.setValue( "name", "VLC Plugin" );
	sendMessage( message );

	return true;
};


////////////////////////////////////////////////////////////////////////////////
//
int init_media_plugin( LLPluginInstance::sendMessageFunction host_send_func,
						void* host_user_data,
						LLPluginInstance::sendMessageFunction *plugin_send_func,
						void **plugin_user_data )
{
	MediaPluginVLC* self = new MediaPluginVLC( host_send_func, host_user_data );
	*plugin_send_func = MediaPluginVLC::staticReceiveMessage;
	*plugin_user_data = ( void* )self;

	return 0;
}

void MediaPluginVLC::Invalidate()
{
	setDirty( 0, 0, mWidth, mHeight );
}


