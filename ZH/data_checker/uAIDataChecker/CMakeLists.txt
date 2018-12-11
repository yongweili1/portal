
CMAKE_MINIMUM_REQUIRED(VERSION  3.0)

project(uAIDataChecker) 

set(CMAKE_CXX_FLAGS "-fPIC")
SET(CMAKE_CXX_FLAGS "-std=c++0x")
SET(CMAKE_SHARED_LINKER_FLAGS "-Xlinker --unresolved-symbols=ignore-in-shared-libs" )

INCLUDE_DIRECTORIES( ../include/ )

include_directories(/usr/local/include/)
INCLUDE_DIRECTORIES(/home/lyw/lyw_wk/external/include/boost-1_60)

#step2: include internal header file directory

INCLUDE_DIRECTORIES(./)

link_directories(/home/lyw/lyw_wk/external/dcmtk_src/build/lib)
link_directories(/home/lyw/anaconda2/lib/)

find_library( date_time libboost_date_time-gcc54-mt-1_60.so ${PROJECT_SOURCE_DIR}/../../external/lib/)
find_library( file_system libboost_filesystem-gcc54-mt-1_60.so  ${PROJECT_SOURCE_DIR}/../../external/lib/)
find_library( _system libboost_system-gcc54-mt-1_60.so  ${PROJECT_SOURCE_DIR}/../../external/lib/)
find_library (1 libdcmjpeg.so /home/lyw/lyw_wk/external/dcmtk_src/build/lib)
 message (${1})

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../bin/)
message("date_time path" ${date_time})
#step4: find all source file
aux_source_directory(./ SRC_LIST)


#step5: create target(execute or library)
add_library(uAIDataChecker SHARED  ${SRC_LIST})
SET_TARGET_PROPERTIES(uAIDataChecker PROPERTIES "-Xlinker" "--unresolved-symbols=ignore-in-shared-libs")

TARGET_LINK_LIBRARIES (uAIDataChecker ${date_time} ${_system} ${file_system} libicudata.so.58 libjpeg.so.9)
TARGET_LINK_LIBRARIES(uAIDataChecker /home/lyw/lyw_wk/external/dcmtk_src/dcmjp2k/libsrc/libdcmjp2k.a) 
 TARGET_LINK_LIBRARIES(uAIDataChecker ${1} libdcmimgle.so libdcmjasp.so  libdcmjpls.so liboflog.so libofstd.so )
 target_link_libraries(uAIDataChecker libdcmdata.so libdcmimage.so  libdcmpstat.so  libdcmjpeg.so  )
 target_link_libraries(uAIDataChecker libijg8.so  libijg12.so  libijg16.so jasper)

#step6: link all required libraries including self developed and 3rd party library

