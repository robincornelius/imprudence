# -*- cmake -*-

project(media_plugin_vlc)

include(00-Common)
include(LLCommon)
include(LLImage)
include(LLPlugin)
include(LLMath)
include(LLRender)
include(LLWindow)
include(Linking)
include(PluginAPI)
include(MediaPluginBase)
include(FindOpenGL)


# set(VLC_FIND_QUIETLY ON)
# set(VLC_FIND_REQUIRED ON)

if (STANDALONE)
#   include(FindVLC) #TODO
else (STANDALONE)
  use_prebuilt_binary(libvlc)
#   set(VLC_LIBRARIES libvlc)
#   set(VLC_INCLUDE_DIRS ${LIBS_PREBUILT_DIR}/include)
endif (STANDALONE)

include_directories(
    ${LLPLUGIN_INCLUDE_DIRS}
    ${MEDIA_PLUGIN_BASE_INCLUDE_DIRS}
    ${LLCOMMON_INCLUDE_DIRS}
    ${LLMATH_INCLUDE_DIRS}
    ${LLIMAGE_INCLUDE_DIRS}
    ${LLRENDER_INCLUDE_DIRS}
    ${LLWINDOW_INCLUDE_DIRS}
)

### media_plugin_vlc

if(NOT CMAKE_SIZEOF_VOID_P MATCHES 4)
	if(WINDOWS)
		add_definitions(/FIXED:NO)
	else(WINDOWS) # not windows therefore gcc LINUX and DARWIN
		add_definitions(-fPIC)
	endif(WINDOWS)
endif (NOT CMAKE_SIZEOF_VOID_P MATCHES 4)

set(media_plugin_vlc_SOURCE_FILES
    media_plugin_vlc.cpp
    )

add_library(media_plugin_vlc
    SHARED
    ${media_plugin_vlc_SOURCE_FILES}
)

if(LINUX)
  target_link_libraries(media_plugin_vlc
    ${LLPLUGIN_LIBRARIES}
    ${MEDIA_PLUGIN_BASE_LIBRARIES}
    ${LLCOMMON_LIBRARIES}
    ${PLUGIN_API_WINDOWS_LIBRARIES}
    vlc
#     vlccore
  )
else(LINUX)
  target_link_libraries(media_plugin_vlc
    ${LLPLUGIN_LIBRARIES}
    ${MEDIA_PLUGIN_BASE_LIBRARIES}
    ${LLCOMMON_LIBRARIES}
    ${PLUGIN_API_WINDOWS_LIBRARIES}
    libvlc
    libvlccore
  )
endif(LINUX)

add_dependencies(media_plugin_vlc
  ${LLPLUGIN_LIBRARIES}
  ${MEDIA_PLUGIN_BASE_LIBRARIES}
  ${LLCOMMON_LIBRARIES}
)

if (WINDOWS)
  set_target_properties(
    media_plugin_vlc
    PROPERTIES
    LINK_FLAGS "/MANIFEST:NO"
    )
endif (WINDOWS)
