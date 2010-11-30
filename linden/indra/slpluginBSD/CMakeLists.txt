project(SLPluginBSD)

include (00-Common)
include (LLSD)
include (EXPAT)
include(Linking)
include(Prebuilt)

include_directories(
    ${LLSD_INCLUDE_DIRS}
    ${EXPAT_INCLUDE_DIRS}
)

if (DARWIN)
    include(CMakeFindFrameworks)
endif (DARWIN)


### SLPluginBSD

set(SLPluginBSD_SOURCE_FILES
    slpluginBSD.cpp
    llpluginmessage.cpp
    llplugininstance.cpp
    llpluginsharedmemory.cpp
    llpluginprocesschild.cpp
    llpluginmessagepipe.cpp
    lltimer.cpp
    )

                        
set(SLPluginBSD_HEADER_FILES
    slpluginBSD.h
     llpluginmessage.h
     llplugininstance.h
     llpluginsharedmemory.h
     llpluginprocesschild.h
     llpluginmessagepipe.h
     lltimer.h

    )
    

  
set_source_files_properties(${SLPluginBSD_HEADER_FILES}
                            PROPERTIES HEADER_FILE_ONLY TRUE)
                            
if (SLPluginBSD_HEADER_FILES)
  list(APPEND SLPluginBSD_SOURCE_FILES ${SLPluginBSD_HEADER_FILES})
endif (SLPluginBSD_HEADER_FILES)

add_executable(SLPluginBSD
    WIN32
    MACOSX_BUNDLE
    ${SLPluginBSD_SOURCE_FILES}
)

target_link_libraries(SLPluginBSD
    wsock32.lib
    ws2_32.lib
    Advapi32.lib
     ${LLSD_LIBRARIES}
    debug ${ARCH_PREBUILT_DIRS_DEBUG}/${EXPAT_LIBRARIES}.lib
    optimized ${ARCH_PREBUILT_DIRS_RELEASE}/${EXPAT_LIBRARIES}.lib
)
