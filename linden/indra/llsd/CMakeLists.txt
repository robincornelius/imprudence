# -*- cmake -*-

project(llsd)

include(00-Common)
include(LLCommon)
include(LLMath)
include(LLMessage)
include(LLXML)

include_directories(
  ${EXPAT_INCLUDE_DIRS}
    )

set(llsd_SOURCE_FILES
	llidl.cpp
	llsd.cpp
	llsdserialize_binary.cpp
	llsdserialize_impl.cpp
	llsdserialize_json.cpp
	llsdserialize_notation.cpp
	llsdserialize_xml.cpp
    )
    
set(llsd_HEADER_FILES
    CMakeLists.txt
	llidl.h
	llsd.h
	llsdserialize.h
	llsdserialize_impl.h
	preprocessor.h
	stdtypes.h
	stub.h
    )

set_source_files_properties(${llsd_HEADER_FILES}
                            PROPERTIES HEADER_FILE_ONLY TRUE)

list(APPEND llsd_SOURCE_FILES ${llsd_HEADER_FILES})

add_library (llsd ${llsd_SOURCE_FILES})
