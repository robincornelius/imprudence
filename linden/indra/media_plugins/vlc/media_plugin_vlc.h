#ifndef _MEDIA_PLUGIN_VLC_H
#define _MEDIA_PLUGIN_VLC_H

typedef enum e_key_event
		{
			KE_KEY_DOWN,
			KE_KEY_REPEAT,
			KE_KEY_UP
		} EKeyEvent;

		typedef enum e_keyboard_modifier
		{
			KM_MODIFIER_NONE = 0x00,
			KM_MODIFIER_SHIFT = 0x01,
			KM_MODIFIER_CONTROL = 0x02,
			KM_MODIFIER_ALT = 0x04,
			KM_MODIFIER_META = 0x08
		} EKeyboardModifier;


class MediaPluginVLC : public MediaPluginBase
{
	public:
		MediaPluginVLC( LLPluginInstance::sendMessageFunction host_send_func, void *host_user_data );
		~MediaPluginVLC();
		static MediaPluginVLC * getInstance(){return sInstance;};

		/*virtual*/ void receiveMessage( const char* message_string );

		void size_change_request(int w,int h);

		unsigned char * mRenderBuffer;
		unsigned char mDummyRenderBuffer[400]; // 10x10 initBuffer

		
	private:
		static MediaPluginVLC * sInstance;
		bool init();
		void update( F64 milliseconds );

		time_t mLastUpdateTime;
		enum Constants { ENumObjects = 10 };
		unsigned char* mBackgroundPixels;

		void LoadURI(std::string uri);

		int mColorR[ ENumObjects ];
		int mColorG[ ENumObjects ];
		int mColorB[ ENumObjects ];
		int mXpos[ ENumObjects ];
		int mYpos[ ENumObjects ];
		int mXInc[ ENumObjects ];
		int mYInc[ ENumObjects ];
		int mBlockSize[ ENumObjects ];

		libvlc_instance_t * inst;
		
		libvlc_state_t mMediaState;
		libvlc_event_manager_t *em;

public:
		libvlc_media_player_t *mp;
		
		bool mSizeChangeRequestSent;
		
	    void Invalidate();

		static void MediaPluginVLC::status_callback(const libvlc_event_t *ev, void *data);

		enum mPlaySetUpStates
		{
			STATE_WAITFMT,
			STATE_GOTFMT,
			STATE_WAITSTOP,
			STATUS_WAITSIZECHANGE,
			STATUS_SIZECHANGECOMPLETE,
			STATUS_DANCEFINISHED
		};

		mPlaySetUpStates mCurrentInitState;

		int mNaturalWidth;
		int mNaturalHeight;

		double mCurrentVolume;

		std::string mNowPlaying;
		std::string mTitle;

		std::list <std::string> mMediaList;

		bool mMoveNextMedia;
};

#endif