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

		void size_change_request(int w,int h,int d);
	
		void setDirty2() { setDirty(0,0,mWidth,mHeight);};
		
	private:
		static MediaPluginVLC * sInstance;
		bool init();
		void update( F64 milliseconds );

		bool mFirstTime;
		void keyEvent(EKeyEvent key_event, int key, EKeyboardModifier modifiers, LLSD native_key_data = LLSD::emptyMap());
		EKeyboardModifier decodeModifiers(std::string &modifiers);

		time_t mLastUpdateTime;
		enum Constants { ENumObjects = 10 };
		unsigned char* mBackgroundPixels;

		int mColorR[ ENumObjects ];
		int mColorG[ ENumObjects ];
		int mColorB[ ENumObjects ];
		int mXpos[ ENumObjects ];
		int mYpos[ ENumObjects ];
		int mXInc[ ENumObjects ];
		int mYInc[ ENumObjects ];
		int mBlockSize[ ENumObjects ];

		bool mMouseButtonDown;
		bool mStopAction;

		libvlc_instance_t * inst;
		libvlc_media_player_t *mp;
		libvlc_media_t *m;
};

#endif