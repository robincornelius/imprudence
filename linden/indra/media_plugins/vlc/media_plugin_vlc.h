#ifndef _MEDIA_PLUGIN_VLC_H
#define _MEDIA_PLUGIN_VLC_H

class MediaPluginVLC : public MediaPluginBase
{
	public: //methods

		enum mPlaySetUpStates
		{
			STATE_WAITFMT,
			STATE_GOTFMT,
			STATE_WAITSTOP,
			STATUS_WAITSIZECHANGE,
			STATUS_SIZECHANGECOMPLETE,
			STATUS_DANCEFINISHED
		};

		MediaPluginVLC( LLPluginInstance::sendMessageFunction host_send_func, void *host_user_data );
		~MediaPluginVLC();
	
		/*virtual*/ void receiveMessage( const char* message_string );

		void sizeChangeRequest(int w,int h);

		void LoadURI(std::string uri);

		void Invalidate();

		static void status_callback(const libvlc_event_t *ev, void *data);

	public: // properties

		libvlc_media_player_t *mMediaPlayer;
		
		bool mSizeChangeRequestSent;
	
		mPlaySetUpStates mCurrentInitState;

		int mNaturalWidth;
		int mNaturalHeight;

		double mCurrentVolume;

		std::string mNowPlaying;
		std::string mTitle;

		std::list <std::string> mMediaList;

		bool mMoveNextMedia;

		bool mPlayingForReal;

		unsigned char * mRenderBuffer;
		unsigned char mDummyRenderBuffer[400]; // 10x10 initBuffer

	private:

		bool init();
		void update( F64 milliseconds );

		time_t mLastUpdateTime;

		libvlc_instance_t * inst;
		
		libvlc_state_t mMediaState;
		libvlc_event_manager_t *em;

};

#endif