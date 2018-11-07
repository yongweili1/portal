CMAKE_MINIMUM_REQUIRED(VERSION  2.8)

#step1: include CMakeDefault.txt 
INCLUDE(../../CMakeDefault.txt)

#step2: include internal header file directory 
INCLUDE_DIRECTORIES(./include ../McsfUtility/include ../include/McsfNetBase )
INCLUDE_DIRECTORIES( ../../../../UIH/include/mcs )

#step3: define macro(preprocessor) needed 
ADD_DEFINITIONS(-DMCSFNETBASE_BUILD_DLL -DULIB)

SET(CMAKE_CXX_FLAGS "-std=gnu++0x")
#step4: find all source file 
AUX_SOURCE_DIRECTORY(src SRC_LIST)

#step5: create target(execute or library) 
ADD_LIBRARY (McsfNetBase SHARED ${SRC_LIST})

#step6: link all required libraries including self developed and 3rd party library 
TARGET_LINK_LIBRARIES (McsfNetBase ${LIBRARY_PATH_ACE} ${LIBRARY_PATH_PROTOBUF} ${LIBRARY_PATH_BOOSTTHREAD} McsfUtility McsfLogger )

